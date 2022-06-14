#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct {
    ngx_uint_t value;
} shardeb_conf_t;

static ngx_int_t shardeb_filter(ngx_http_request_t *r);
static void *shardeb_create_loc_conf(ngx_conf_t *cf);
static char *shardeb_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static char *shards(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *clusters(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t shardeb_commands[] = {
    {ngx_string("shards"),
     NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF |
         NGX_CONF_TAKE2,
     shards, NGX_HTTP_LOC_CONF_OFFSET, 0, NULL},
    {ngx_string("clusters"),
     NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF |
         NGX_CONF_TAKE2,
     clusters, NGX_HTTP_LOC_CONF_OFFSET, 0, NULL},
};

static ngx_http_module_t shardeb_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    shardeb_create_loc_conf, /* create location configuration */
    shardeb_merge_loc_conf   /* merge location configuration */
};

ngx_module_t shardeb_module = {NGX_MODULE_V1,
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

/* header filter handler */

// static ngx_int_t shardeb_filter(ngx_http_request_t *r)
// {
//     ngx_table_elt_t *h;
//     shardeb_conf_t *slcf;

//     ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
//                    "http set_header handler");

//     slcf = ngx_http_get_module_loc_conf(r, shardeb_module);

//     /*
//      * if no header is defined for the location,
//      * proceed to the next header filter in chain
//      */

//     if (slcf->value == NULL) {
//         return ngx_http_next_header_filter(r);
//     }

//     /* add header to output */

//     h = ngx_list_push(&r->headers_out.headers);
//     if (h == NULL) {
//         return NGX_ERROR;
//     }

//     h->hash = 1;
//     h->value = slcf->value;

//     /* proceed to the next handler in chain */

//     return ngx_http_next_header_filter(r);
// }

static void *shardeb_create_loc_conf(ngx_conf_t *cf)
{
    shardeb_conf_t *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(shardeb_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    return conf;
}

static char *shardeb_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    shardeb_conf_t *prev = parent;
    shardeb_conf_t *conf = child;

    if (conf->value == NULL) {
        conf->value = prev->value;
    }

    return NGX_CONF_OK;
}

static char *shards(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    shardeb_conf_t *slcf = conf;

    ngx_uint_t *value;

    if (slcf->value) {
        return "is duplicate";
    }

    value = cf->args->elts;

    slcf->value = value[1];

    return NGX_CONF_OK;
}

static char *clusters(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    shardeb_conf_t *slcf = conf;

    ngx_uint_t *value;

    if (slcf->value) {
        return "is duplicate";
    }

    value = cf->args->elts;

    slcf->value = value[1];

    return NGX_CONF_OK;
}
