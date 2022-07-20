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
 * mod_example5.c
 * 
 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * mod_example_config.c
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


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

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Configuration structure
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct
{
    char    context[256];
    char    path[256];
    int     typeOfAction;
    int     enabled;
} example5_config;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Prototypes
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

static int    example5_handler(request_rec *r);
const char    *example5_set_enabled(cmd_parms *cmd, void *cfg, const char *arg);
const char    *example5_set_path(cmd_parms *cmd, void *cfg, const char *arg);
const char    *example5_set_action(cmd_parms *cmd, void *cfg, const char *arg1, const char *arg2);
void          *create_dir_conf(apr_pool_t *pool, char *context);
void          *merge_dir_conf(apr_pool_t *pool, void *BASE, void *ADD);
static void   register_hooks(apr_pool_t *pool);

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Configuration directives
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

static const command_rec    directives[] =
{
    AP_INIT_TAKE1("exampleEnabled", example5_set_enabled, NULL, ACCESS_CONF, "Enable or disable mod_example"),
    AP_INIT_TAKE1("examplePath", example5_set_path, NULL, ACCESS_CONF, "The path to whatever"),
    AP_INIT_TAKE2("exampleAction", example5_set_action, NULL, ACCESS_CONF, "Special action value!"),
    { NULL }
};

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Our name tag
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

module AP_MODULE_DECLARE_DATA    example5_module =
{
    STANDARD20_MODULE_STUFF,
    create_dir_conf,    /* Per-directory configuration handler */
    merge_dir_conf,     /* Merge handler for per-directory configurations */
    NULL,               /* Per-server configuration handler */
    NULL,               /* Merge handler for per-server configurations */
    directives,         /* Any directives we may have for httpd */
    register_hooks      /* Our hook registering function */
};

/*
 =======================================================================================================================
    Hook registration function
 =======================================================================================================================
 */
static void register_hooks(apr_pool_t *pool)
{
    ap_hook_handler(example5_handler, NULL, NULL, APR_HOOK_LAST);
}

/*
 =======================================================================================================================
    Our example web service handler
 =======================================================================================================================
 */
static int example5_handler(request_rec *r)
{
    if(!r->handler || strcmp(r->handler, "example5-handler")) return(DECLINED);

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    example5_config    *config = (example5_config *) ap_get_module_config(r->per_dir_config, &example5_module);
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ap_set_content_type(r, "text/plain");
    ap_rprintf(r, "Enabled: %u\n", config->enabled);
    ap_rprintf(r, "Path: %s\n", config->path);
    ap_rprintf(r, "TypeOfAction: %x\n", config->typeOfAction);
    ap_rprintf(r, "Context: %s\n", config->context);
    return OK;
}

/*
 =======================================================================================================================
    Handler for the "exampleEnabled" directive
 =======================================================================================================================
 */
const char *example5_set_enabled(cmd_parms *cmd, void *cfg, const char *arg)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    example5_config    *conf = (example5_config *) cfg;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(conf)
    {
        if(!strcasecmp(arg, "on"))
            conf->enabled = 1;
        else
            conf->enabled = 0;
    }

    return NULL;
}

/*
 =======================================================================================================================
    Handler for the "examplePath" directive
 =======================================================================================================================
 */
const char *example5_set_path(cmd_parms *cmd, void *cfg, const char *arg)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    example5_config    *conf = (example5_config *) cfg;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(conf)
    {
        strcpy(conf->path, arg);
    }

    return NULL;
}

/*
 =======================================================================================================================
    Handler for the "exampleAction" directive ;
    Let's pretend this one takes one argument (file or db), and a second (deny or allow), ;
    and we store it in a bit-wise manner.
 =======================================================================================================================
 */
const char *example5_set_action(cmd_parms *cmd, void *cfg, const char *arg1, const char *arg2)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    example5_config    *conf = (example5_config *) cfg;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(conf)
    {
        {
            if(!strcasecmp(arg1, "file"))
                conf->typeOfAction = 0x01;
            else
                conf->typeOfAction = 0x02;
            if(!strcasecmp(arg2, "deny"))
                conf->typeOfAction += 0x10;
            else
                conf->typeOfAction += 0x20;
        }
    }

    return NULL;
}

/*
 =======================================================================================================================
    Function for creating new configurations for per-directory contexts
 =======================================================================================================================
 */
void *create_dir_conf(apr_pool_t *pool, char *context)
{
    context = context ? context : "Newly created configuration";

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    example5_config    *cfg = apr_pcalloc(pool, sizeof(example5_config));
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(cfg)
    {
        {
            /* Set some default values */
            strcpy(cfg->context, context);
            cfg->enabled = 0;
            memset(cfg->path, 0, 256);
            cfg->typeOfAction = 0x00;
        }
    }

    return cfg;
}

/*
 =======================================================================================================================
    Merging function for configurations
 =======================================================================================================================
 */
void *merge_dir_conf(apr_pool_t *pool, void *BASE, void *ADD)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    example5_config    *base = (example5_config *) BASE;
    example5_config    *add = (example5_config *) ADD;
    example5_config    *conf = (example5_config *) create_dir_conf(pool, "Merged configuration");
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    conf->enabled = (add->enabled == 0) ? base->enabled : add->enabled;
    conf->typeOfAction = add->typeOfAction ? add->typeOfAction : base->typeOfAction;
    strcpy(conf->path, strlen(add->path) ? add->path : base->path);
    return conf;
}
