--complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_mqtt" to load this file. \quit

CREATE OR REPLACE FUNCTION mqtt_post(
    host TEXT, 
    topic TEXT, 
    payload TEXT, 
    client_id TEXT DEFAULT 'pg_mqtt', 
    user_name TEXT DEFAULT NULL,
    user_pwd TEXT DEFAULT NULL,
    port INTEGER DEFAULT 1183,
    use_tls BOOLEAN DEFAULT TRUE) 
    RETURNS INTEGER
AS '$libdir/pg_mqtt'
LANGUAGE C IMMUTABLE STRICT;