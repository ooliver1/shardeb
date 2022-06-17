#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_int_t shards;
    ngx_int_t clusters;
    ngx_str_t query_param;
} shardeb_conf_t;

ngx_str_t GUILD = ngx_string("guild");

static ngx_int_t shardeb_init(ngx_conf_t *cf);
static void *shardeb_create_loc_conf(ngx_conf_t *cf);
static char *shardeb_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static char *shardeb(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t shardeb_commands[] = {
    {ngx_string("shardeb"), NGX_HTTP_LOC_CONF | NGX_CONF_TAKE3, shardeb,
     NGX_HTTP_LOC_CONF_OFFSET, 0, NULL},
    ngx_null_command};

static ngx_http_module_t shardeb_ctx = {
    NULL,         /* preconfiguration */
    shardeb_init, /* postconfiguration */

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

static ngx_int_t shardeb_handler(ngx_http_request_t *r) {
    ngx_conf_t *cf = ngx_http_get_module_loc_conf(r, shardeb_module);
    shardeb_conf_t *conf =
        ngx_http_conf_get_module_loc_conf(cf, shardeb_module);
    ngx_str_t *name = &conf->query_param;
    ngx_int_t guild = ngx_http_get_variable_index(cf, name);

    int shard = (guild << 22) % conf->shards;
    uintptr_t cluster = (shard) / (conf->shards / conf->clusters);

    ngx_http_variable_t *var =
        ngx_http_add_variable(cf, &GUILD, NGX_HTTP_VAR_CHANGEABLE);

    if (var == NULL) {
        return NGX_ERROR;
    }

    var->data = cluster;
    return NGX_OK;
}

static char *shardeb(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    shardeb_conf_t *config = conf;

    ngx_int_t *check;
    ngx_int_t clusters;
    ngx_int_t shards;
    ngx_str_t *value;

    check = (ngx_int_t *)(config + cmd->offset);

    if (*check != NGX_CONF_UNSET) {
        return "is duplicate";
    }

    value = cf->args->elts;

    clusters = ngx_atoi(value[2].data, value[2].len);
    shards = ngx_atoi(value[2].data, value[2].len);

    if (clusters == NGX_ERROR || shards == NGX_ERROR) {
        return "invalid number";
    }

    config->clusters = clusters;
    config->shards = shards;
    config->query_param = value[3];

    return NGX_CONF_OK;
}

static ngx_int_t shardeb_init(ngx_conf_t *cf) {
    ngx_http_core_loc_conf_t *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = shardeb_handler;

    return NGX_OK;
}
