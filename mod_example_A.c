/* Licensed to the Apache Software Foundation (ASF) under one or more
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
 * mod_example_A.c
 *
 */

/* Include the required headers from httpd */
#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"

/* Define prototypes of our functions in this module */
static void register_hooks(apr_pool_t *pool);
static int example_A_handler(request_rec *r);

/* Define our module as an entity and assign a function for registering hooks  */

module AP_MODULE_DECLARE_DATA   example_A_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,            // Per-directory configuration handler
    NULL,            // Merge handler for per-directory configurations
    NULL,            // Per-server configuration handler
    NULL,            // Merge handler for per-server configurations
    NULL,            // Any directives we may have for httpd
    register_hooks   // Our hook registering function
};


/* register_hooks: Adds a hook to the httpd process */
static void register_hooks(apr_pool_t *pool) 
{
    
    /* Hook the request handler */
    ap_hook_handler(example_A_handler, NULL, NULL, APR_HOOK_LAST);
}

/* The handler function for our module.
 * This is where all the fun happens!
 */
static int example_A_handler(request_rec *r)
{
    if (!r->handler || strcmp(r->handler, "exampleA-handler")) return (DECLINED);

    /* Set the appropriate content type */
    ap_set_content_type(r, "text/html");

    /* Print out the IP address of the client connecting to us: */
    ap_rprintf(r, "<h2>Hello, %s!</h2>", r->useragent_ip);
    
    /* If we were reached through a GET or a POST request, be happy, else sad. */
    if ( !strcmp(r->method, "POST") || !strcmp(r->method, "GET") ) {
        ap_rputs("You used a GET or a POST method, that makes us happy!<br/>", r);
    }
    else {
        ap_rputs("You did not use POST or GET, that makes us sad :(<br/>", r);
    }

    /* Lastly, if there was a query string, let's print that too! */
    if (r->args) {
        ap_rprintf(r, "Your query string was: %s", r->args);
    }
    return OK;
}
