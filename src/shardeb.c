#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_int_t shards;
    ngx_int_t clusters;
    ngx_str_t query_param;
} shardeb_conf_t;

#define UNUSED(x) (void)(x)

static void *shardeb_create_loc_conf(ngx_conf_t *cf);
static char *shardeb_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static char *shardeb(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t guild_variable_handler(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data
);
static ngx_int_t shardeb_add_vars(ngx_conf_t *cf);

static ngx_command_t shardeb_commands[] = {
    {ngx_string("shardeb"), NGX_HTTP_LOC_CONF | NGX_CONF_TAKE3, shardeb,
     NGX_HTTP_LOC_CONF_OFFSET, 0, NULL},
    ngx_null_command};

static ngx_str_t guild_variable_name = ngx_string("guild");

static ngx_http_module_t shardeb_ctx = {
    shardeb_add_vars, /* preconfiguration */
    NULL,             /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    shardeb_create_loc_conf, /* create location configuration */
    shardeb_merge_loc_conf   /* merge location configuration */
};

ngx_module_t shardeb_module = {
    NGX_MODULE_V1,
    &shardeb_ctx,     /* module context */
    shardeb_commands, /* module directives */
    NGX_HTTP_MODULE,  /* module type */
    NULL,             /* init master */
    NULL,             /* init module */
    NULL,             /* init process */
    NULL,             /* init thread */
    NULL,             /* exit thread */
    NULL,             /* exit process */
    NULL,             /* exit master */
    NGX_MODULE_V1_PADDING};

static void *shardeb_create_loc_conf(ngx_conf_t *cf) {
    shardeb_conf_t *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(shardeb_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}

static ngx_int_t shardeb_add_vars(ngx_conf_t *cf) {
    ngx_http_variable_t *var;

    var = ngx_http_add_variable(cf, &guild_variable_name, 0);

    if (var == NULL) {
        return NGX_ERROR;
    }

    var->get_handler = guild_variable_handler;

    return NGX_OK;
}

static char *shardeb_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child) {
    shardeb_conf_t *prev = parent;
    shardeb_conf_t *conf = child;

    if (conf->query_param.data == NULL) {
        conf->shards = prev->shards;
        conf->clusters = prev->clusters;
        conf->query_param = prev->query_param;
    }

    return NGX_CONF_OK;
}

static char *shardeb(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    shardeb_conf_t *config = conf;

    ngx_int_t *check;
    ngx_int_t clusters;
    ngx_int_t shards;
    ngx_str_t *value;

    check = (ngx_int_t *)(config + cmd->offset);

    if (check == NGX_CONF_UNSET_PTR) {
        return "is duplicate";
    }

    value = cf->args->elts;

    clusters = ngx_atoi(value[1].data, value[1].len);
    shards = ngx_atoi(value[2].data, value[2].len);

    if (clusters == NGX_ERROR || shards == NGX_ERROR) {
        return "invalid number";
    }

    ngx_str_t guild_var_name = value[3];
    char arg_constant[] = "arg_";

    char *s = malloc(
        snprintf(NULL, 0, "%s %s", arg_constant, guild_var_name.data) + 1
    );

    ngx_str_t guild_arg_name = ngx_string(s);

    config->clusters = clusters;
    config->shards = shards;
    config->query_param = value[3];
    UNUSED(guild_arg_name);

    return NGX_CONF_OK;
}

static ngx_int_t guild_variable_handler(
    ngx_http_request_t *r, ngx_http_variable_value_t *var, uintptr_t data
) {
    shardeb_conf_t *conf = ngx_http_get_module_loc_conf(r, shardeb_module);

    u_char *query = conf->query_param.data;
    ngx_uint_t hash = ngx_hash_strlow(query, query, sizeof(query));

    ngx_http_variable_value_t *guild_var =
        ngx_http_get_variable(r, &conf->query_param, hash);
    ngx_int_t guild = ngx_atoi(guild_var->data, guild_var->len);

    if (guild == NGX_ERROR) {
        goto not_found;
    }

    int shard = (guild << 22) % conf->shards;
    uintptr_t cluster = (shard) / (conf->shards / conf->clusters);

    if (var == NULL) {
        return NGX_ERROR;
    }

    var->data = (u_char *)cluster;

    return NGX_OK;

not_found:

    var->not_found = 1;

    return NGX_OK;
}