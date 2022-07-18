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
 * mod_example_4.c
 * 
 */

/* mod_example_config_simple.c: */
#include <stdio.h>
#include "apr_hash.h"
#include "ap_config.h"
#include "ap_provider.h"
#include "httpd.h"
#include "http_core.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"
#include "http_request.h"

/*
 ==============================================================================
 Our configuration prototype and declaration:
 ==============================================================================
 */
typedef struct {
    int         enabled;      /* Enable or disable our module */
    const char *path;         /* Some path to...something */
    int         typeOfAction; /* 1 means action A, 2 means action B and so on */
} example4_config;

static example4_config config;

/*
 ==============================================================================
 Our directive handlers:
 ==============================================================================
 */
/* Handler for the "exampleEnabled" directive */
const char *example4_set_enabled(cmd_parms *cmd, void *cfg, const char *arg)
{
    if(!strcasecmp(arg, "on")) config.enabled = 1;
    else config.enabled = 0;
    return NULL;
}

/* Handler for the "examplePath" directive */
const char *example4_set_path(cmd_parms *cmd, void *cfg, const char *arg)
{
    config.path = arg;
    return NULL;
}

/* Handler for the "exampleAction" directive */
/* Let's pretend this one takes one argument (file or db), and a second (deny or allow), */
/* and we store it in a bit-wise manner. */
const char *example4_set_action(cmd_parms *cmd, void *cfg, const char *arg1, const char *arg2)
{
    if(!strcasecmp(arg1, "file")) config.typeOfAction = 0x01;
    else config.typeOfAction = 0x02;
    
    if(!strcasecmp(arg2, "deny")) config.typeOfAction += 0x10;
    else config.typeOfAction += 0x20;
    return NULL;
}

/*
 ==============================================================================
 The directive structure for our name tag:
 ==============================================================================
 */
static const command_rec        example4_directives[] =
{
    AP_INIT_TAKE1("exampleEnabled", example4_set_enabled, NULL, RSRC_CONF, "Enable or disable mod_example"),
    AP_INIT_TAKE1("examplePath", example4_set_path, NULL, RSRC_CONF, "The path to whatever"),
    AP_INIT_TAKE2("exampleAction", example4_set_action, NULL, RSRC_CONF, "Special action value!"),
    { NULL }
};
/*
 ==============================================================================
 Our module handler:
 ==============================================================================
 */
static int example4_handler(request_rec *r)
{
    if(!r->handler || strcmp(r->handler, "example4-handler")) return(DECLINED);
    ap_set_content_type(r, "text/plain");
    ap_rprintf(r, "Enabled: %u\n", config.enabled);
    ap_rprintf(r, "Path: %s\n", config.path);
    ap_rprintf(r, "TypeOfAction: %x\n", config.typeOfAction);
    return OK;
}

/*
 ==============================================================================
 The hook registration function (also initializes the default config values):
 ==============================================================================
 */
static void register_hooks(apr_pool_t *pool) 
{
    config.enabled = 1;
    config.path = "/foo/bar";
    config.typeOfAction = 3;
    ap_hook_handler(example4_handler, NULL, NULL, APR_HOOK_LAST);
}
/*
 ==============================================================================
 Our module name tag:
 ==============================================================================
 */
module AP_MODULE_DECLARE_DATA   example4_module =
{
    STANDARD20_MODULE_STUFF,
    NULL,               /* Per-directory configuration handler */
    NULL,               /* Merge handler for per-directory configurations */
    NULL,               /* Per-server configuration handler */
    NULL,               /* Merge handler for per-server configurations */
    example4_directives, /* Any directives we may have for httpd */
    register_hooks      /* Our hook registering function */
};
