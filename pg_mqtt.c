#include "postgres.h"

#include "fmgr.h"
#include "funcapi.h"
#include "utils/memutils.h"
#include "utils/builtins.h"
#include <stdio.h>
#include <stdlib.h>
#if (PG_VERSION_NUM >= 90300)
#include "access/htup_details.h"
#endif

#if PG_VERSION_NUM >= 110000
	#ifndef TRUE
		#define TRUE true
	#endif
	#ifndef FALSE
		#define FALSE false
	#endif
#endif

#include "MQTTClient.h"


#define URL_MAX_LENGHT  1024
#define QOS             1
#define TIMEOUT         10000L

#define GET_STR( textp ) DatumGetCString( DirectFunctionCall1( textout, PointerGetDatum( textp )))

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(mqtt_post);


Datum mqtt_post(PG_FUNCTION_ARGS)
{

    char * host         =  GET_STR( PG_GETARG_TEXT_P( 0 ));
    char * topic        =  GET_STR( PG_GETARG_TEXT_P( 1 ));
    char * payload      =  GET_STR( PG_GETARG_TEXT_P( 2 ));
    char * client_id    =  GET_STR( PG_GETARG_TEXT_P( 3 ));
    int port            =  PG_GETARG_INT32( 6 );
    bool use_tls        =  PG_GETARG_BOOL( 7 );    
    char url[URL_MAX_LENGHT];
    int rc;
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    

    if (!PG_ARGISNULL(4) && !PG_ARGISNULL(5)) 
    {
        conn_opts.username      = GET_STR( PG_GETARG_TEXT_P( 4 ));
        conn_opts.password     =  GET_STR( PG_GETARG_TEXT_P( 5 ));
    }

    if(use_tls)
    {
        MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;
        conn_opts.ssl = &ssl_opts;
    }

    snprintf(url, URL_MAX_LENGHT, "tcp://%s:%d", host, port);
    
    MQTTClient_create(&client, 
        url, 
        client_id, 
        MQTTCLIENT_PERSISTENCE_NONE, 
        NULL);
    
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        PG_RETURN_INT32(rc);
    }

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

    MQTTClient_disconnect(client, 10000);
    
    MQTTClient_destroy(&client);
    
    PG_RETURN_INT32(rc);
}