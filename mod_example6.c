/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2022 Pierre Forstmann.
 *
 * mod_example6.c
 *
 */
#include "apr_buckets.h"
#include "util_filter.h"

#include "ap_config.h"
#include "ap_provider.h"
#include "apr_strings.h"
#include "httpd.h"
#include "http_core.h"
#include "http_config.h"
#include "http_log.h"
#include "http_connection.h"
#include "http_protocol.h"
#include "http_request.h"
#include "ap_mmn.h"
#include "apr_tables.h"
#include "util_script.h"
#include "apr_uri.h"

#ifdef APLOG_USE_MODULE
APLOG_USE_MODULE(example6);
#endif

module AP_MODULE_DECLARE_DATA example6;


typedef struct {
    const char *key;
    const char *value;
} keyValuePair;

typedef struct {
    int argc;
    keyValuePair *kvp;
} module_config;


/*
 * Main apache functions
 */

static int example6_cbf(void *rec, const char *key, const char *value)
{
    module_config *data;
    int argc;


    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, ((request_rec *)rec)->server, "example6_handler: example6_cbf: key=%s value=%s", key, value);
    ap_rprintf(rec, "key=%s<br>", key);
    ap_rprintf(rec, "value=%s<br>", value);

    data = (module_config *)ap_get_module_config(((request_rec *)rec)->request_config, &example6_module);
    argc = data->argc;
    argc++;
    data->argc = argc;
    ap_set_module_config(((request_rec *)rec)->request_config, &example6_module, data);

    return 1;
}

keyValuePair *readPost(request_rec *r) {
    apr_array_header_t *pairs = NULL;
    apr_off_t len;
    apr_size_t size;
    int res;
    int i = 0;
    char *buffer;
    keyValuePair *kvp;

    res = ap_parse_form_data(r, NULL, &pairs, -1, HUGE_STRING_LEN);
    if (res != OK)
	ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "mod_example6: ap_parse_form rc=%d\n", res);
    if ( pairs == NULL)
	ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "mod_example6: ap_parse_form pairs=NULL\n");
    if (res != OK || !pairs) return NULL; /* Return NULL if we failed or if there are is no POST data */

    kvp = apr_pcalloc(r->pool, sizeof(keyValuePair) * (pairs->nelts + 1));
    while (pairs && !apr_is_empty_array(pairs)) {
        ap_form_pair_t *pair = (ap_form_pair_t *) apr_array_pop(pairs);
        apr_brigade_length(pair->value, 1, &len);
        size = (apr_size_t) len;
        buffer = apr_palloc(r->pool, size + 1);
        apr_brigade_flatten(pair->value, buffer, &size);
        buffer[len] = 0;
        kvp[i].key = apr_pstrdup(r->pool, pair->name);
        kvp[i].value = buffer;
        i++;
    }
    return kvp;
}

static int example6_handler(request_rec *r)
{
    apr_uri_t 	uri;
    apr_status_t status;
    char *uri_root;
    char *endof_uri_root;
    apr_table_t *args = NULL;
    module_config *data;
    keyValuePair *formData;
    int i;

    /*
     * use request module_config to have argc thread safe
     */

    ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: entry");
    if (strcmp(r->handler, "example6-handler")) {
        return DECLINED;
    }

    if (r->method_number == M_GET) 
        ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: r->method_number=M_GET");
    else if (r->method_number == M_POST) 
        ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: r->method_number=M_POST");
    else {
        ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: method_number=%d", r->method_number);
        return DECLINED;
    }

    if (r->args != NULL)
        ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: r->args=%s",r->args);
    else
        ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: r->args=NULL");

    ap_args_to_table(r, &args);

    formData = readPost(r);
    if (formData) {
        int i;
        for (i = 0; ; i++) {
            if (formData[i].key && formData[i].value) {
                ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: %s=%s", formData[i].key, formData[i].value);
            } else if (formData[i].key) {
                ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: %s", formData[i].key);
            } else if (formData[i].value) {
                ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: %s", formData[i].value);
            } else {
                break;
            }
        }
    } else
        ap_log_error(APLOG_MARK, APLOG_DEBUG, 0, r->server, "example6_handler: formData=NULL");
	    
    ap_set_content_type(r, "text/html");
    ap_rprintf(r, "<h3>example6</h3>");

    if (r->method_number == M_GET) {
        ap_rprintf(r, "<h4>method=GET</h4>");

	status = apr_uri_parse(r->pool, r->uri, &uri);
        uri_root = (char *)(uri.path);
        endof_uri_root = strchr(uri.path, '?');
	if (endof_uri_root != NULL)
	   uri_root[endof_uri_root - uri_root] = '\0';
        ap_rprintf(r, "uri_root=%s<br>", uri_root);

    	data = apr_palloc(r->pool, sizeof(module_config));
        data->argc = 0; 
        ap_set_module_config(r->request_config, &example6_module, data);
        apr_table_do(example6_cbf, r, args, NULL);
        data = ap_get_module_config(r->request_config, &example6_module);

    } else if (r->method_number == M_POST) {
        ap_rprintf(r, "<h4>method=POST</h4>");

	status = apr_uri_parse(r->pool, r->uri, &uri);
        uri_root = (char *)(uri.path);
        endof_uri_root = strchr(uri.path, '?');
	if (endof_uri_root != NULL)
	   uri_root[endof_uri_root - uri_root] = '\0';
        ap_rprintf(r, "uri_root=%s<br>", uri_root);

        if (formData) {
            for (i = 0 ; ; i++) {
   	        if (formData[i].key && formData[i].value) {
                     ap_rprintf(r, "key=%s<br>", formData[i].key);
                     ap_rprintf(r, "value=%s<br>", formData[i].value);
                } else if (formData[i].key) {
                     ap_rprintf(r, "key=%s<br>", formData[i].key);
                } else if (formData[i].value) {
                     ap_rprintf(r, "value=%s<br>", formData[i].value);
               } else {
                    break;
               }
	      }
	   }
	}

    return OK;

}

static void register_hooks(apr_pool_t *p)
{
  ap_hook_handler(example6_handler, NULL, NULL, APR_HOOK_LAST);
}

module AP_MODULE_DECLARE_DATA example6_module = {
    STANDARD20_MODULE_STUFF,
    NULL,			/* create per-directory config structure */
    NULL,		        /* merge per-directory config structures */
    NULL,	                /* create per-server config structure */
    NULL,		        /* merge per-server config structures */
    NULL,			/* command apr_table_t */
    register_hooks		/* register hooks */
};
