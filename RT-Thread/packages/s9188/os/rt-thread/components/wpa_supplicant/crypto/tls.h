/******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/


#ifndef TLS_H
#define TLS_H
#include "utils/common.h"
struct tls_connection;

struct tls_random {
	const u8 *client_random;
	size_t client_random_len;
	const u8 *server_random;
	size_t server_random_len;
};

enum tls_event {
	TLS_CERT_CHAIN_SUCCESS,
	TLS_CERT_CHAIN_FAILURE,
	TLS_PEER_CERTIFICATE,
	TLS_ALERT
};

enum tls_fail_reason {
	TLS_FAIL_UNSPECIFIED = 0,
	TLS_FAIL_UNTRUSTED = 1,
	TLS_FAIL_REVOKED = 2,
	TLS_FAIL_NOT_YET_VALID = 3,
	TLS_FAIL_EXPIRED = 4,
	TLS_FAIL_SUBJECT_MISMATCH = 5,
	TLS_FAIL_ALTSUBJECT_MISMATCH = 6,
	TLS_FAIL_BAD_CERTIFICATE = 7,
	TLS_FAIL_SERVER_CHAIN_PROBE = 8,
	TLS_FAIL_DOMAIN_SUFFIX_MISMATCH = 9,
	TLS_FAIL_DOMAIN_MISMATCH = 10,
};

#define TLS_MAX_ALT_SUBJECT 10

union tls_event_data {
	struct {
		int depth;
		const char *subject;
		enum tls_fail_reason reason;
		const char *reason_txt;
		const struct wpabuf *cert;
	} cert_fail;

	struct {
		int depth;
		const char *subject;
		const struct wpabuf *cert;
		const u8 *hash;
		size_t hash_len;
		const char *altsubject[TLS_MAX_ALT_SUBJECT];
		int num_altsubject;
	} peer_cert;

	struct {
		int is_local;
		const char *type;
		const char *description;
	} alert;
};

struct tls_config {
	const char *opensc_engine_path;
	const char *pkcs11_engine_path;
	const char *pkcs11_module_path;
	int fips_mode;
	int cert_in_cb;
	const char *openssl_ciphers;
	unsigned int tls_session_lifetime;

	void (*event_cb) (void *ctx, enum tls_event ev,
					  union tls_event_data * data);
	void *cb_ctx;
};

#define TLS_CONN_ALLOW_SIGN_RSA_MD5 BIT(0)
#define TLS_CONN_DISABLE_TIME_CHECKS BIT(1)
#define TLS_CONN_DISABLE_SESSION_TICKET BIT(2)
#define TLS_CONN_REQUEST_OCSP BIT(3)
#define TLS_CONN_REQUIRE_OCSP BIT(4)
#define TLS_CONN_DISABLE_TLSv1_1 BIT(5)
#define TLS_CONN_DISABLE_TLSv1_2 BIT(6)
#define TLS_CONN_EAP_FAST BIT(7)
#define TLS_CONN_DISABLE_TLSv1_0 BIT(8)

struct tls_connection_params {
	const char *ca_cert;
	const u8 *ca_cert_blob;
	size_t ca_cert_blob_len;
	const char *ca_path;
	const char *subject_match;
	const char *altsubject_match;
	const char *suffix_match;
	const char *domain_match;
	const char *client_cert;
	const u8 *client_cert_blob;
	size_t client_cert_blob_len;
	const char *private_key;
	const u8 *private_key_blob;
	size_t private_key_blob_len;
	const char *private_key_passwd;
	const char *dh_file;
	const u8 *dh_blob;
	size_t dh_blob_len;

	int engine;
	const char *engine_id;
	const char *pin;
	const char *key_id;
	const char *cert_id;
	const char *ca_cert_id;
	const char *openssl_ciphers;

	unsigned int flags;
	const char *ocsp_stapling_response;
};

void *tls_init(const struct tls_config *conf);

void tls_deinit(void *tls_ctx);

int tls_get_errors(void *tls_ctx);

struct tls_connection *tls_connection_init(void *tls_ctx);

void tls_connection_deinit(void *tls_ctx, struct tls_connection *conn);

int tls_connection_established(void *tls_ctx, struct tls_connection *conn);

int tls_connection_shutdown(void *tls_ctx, struct tls_connection *conn);

enum {
	TLS_SET_PARAMS_ENGINE_PRV_BAD_PIN = -4,
	TLS_SET_PARAMS_ENGINE_PRV_VERIFY_FAILED = -3,
	TLS_SET_PARAMS_ENGINE_PRV_INIT_FAILED = -2
};

int __must_check
tls_connection_set_params(void *tls_ctx, struct tls_connection *conn,
						  const struct tls_connection_params *params);

int __must_check tls_global_set_params(void *tls_ctx,
									   const struct tls_connection_params
									   *params);

int __must_check tls_global_set_verify(void *tls_ctx, int check_crl);

int __must_check tls_connection_set_verify(void *tls_ctx,
										   struct tls_connection *conn,
										   int verify_peer,
										   unsigned int flags,
										   const u8 * session_ctx,
										   size_t session_ctx_len);

int __must_check tls_connection_get_random(void *tls_ctx,
										   struct tls_connection *conn,
										   struct tls_random *data);

int __must_check tls_connection_prf(void *tls_ctx,
									struct tls_connection *conn,
									const char *label,
									int server_random_first,
									int skip_keyblock,
									u8 * out, size_t out_len);

struct wpabuf *tls_connection_handshake(void *tls_ctx,
										struct tls_connection *conn,
										const struct wpabuf *in_data,
										struct wpabuf **appl_data);

struct wpabuf *tls_connection_handshake2(void *tls_ctx,
										 struct tls_connection *conn,
										 const struct wpabuf *in_data,
										 struct wpabuf **appl_data,
										 int *more_data_needed);

struct wpabuf *tls_connection_server_handshake(void *tls_ctx,
											   struct tls_connection *conn,
											   const struct wpabuf *in_data,
											   struct wpabuf **appl_data);

struct wpabuf *tls_connection_encrypt(void *tls_ctx,
									  struct tls_connection *conn,
									  const struct wpabuf *in_data);

struct wpabuf *tls_connection_decrypt(void *tls_ctx,
									  struct tls_connection *conn,
									  const struct wpabuf *in_data);

struct wpabuf *tls_connection_decrypt2(void *tls_ctx,
									   struct tls_connection *conn,
									   const struct wpabuf *in_data,
									   int *more_data_needed);

int tls_connection_resumed(void *tls_ctx, struct tls_connection *conn);

enum {
	TLS_CIPHER_NONE,
	TLS_CIPHER_RC4_SHA,
	TLS_CIPHER_AES128_SHA,
	TLS_CIPHER_RSA_DHE_AES128_SHA,
	TLS_CIPHER_ANON_DH_AES128_SHA
};

int __must_check tls_connection_set_cipher_list(void *tls_ctx,
												struct tls_connection *conn,
												u8 * ciphers);

int __must_check tls_get_version(void *tls_ctx, struct tls_connection *conn,
								 char *buf, size_t buflen);

int __must_check tls_get_cipher(void *tls_ctx, struct tls_connection *conn,
								char *buf, size_t buflen);

int __must_check tls_connection_enable_workaround(void *tls_ctx,
												  struct tls_connection *conn);

int __must_check tls_connection_client_hello_ext(void *tls_ctx,
												 struct tls_connection *conn,
												 int ext_type, const u8 * data,
												 size_t data_len);

int tls_connection_get_failed(void *tls_ctx, struct tls_connection *conn);

int tls_connection_get_read_alerts(void *tls_ctx, struct tls_connection *conn);

int tls_connection_get_write_alerts(void *tls_ctx, struct tls_connection *conn);

typedef int (*tls_session_ticket_cb)
 (void *ctx, const u8 * ticket, size_t len, const u8 * client_random,
  const u8 * server_random, u8 * master_secret);

int __must_check tls_connection_set_session_ticket_cb(void *tls_ctx,
													  struct tls_connection
													  *conn,
													  tls_session_ticket_cb cb,
													  void *ctx);

void tls_connection_set_log_cb(struct tls_connection *conn,
							   void (*log_cb) (void *ctx, const char *msg),
							   void *ctx);

#define TLS_BREAK_VERIFY_DATA BIT(0)
#define TLS_BREAK_SRV_KEY_X_HASH BIT(1)
#define TLS_BREAK_SRV_KEY_X_SIGNATURE BIT(2)
#define TLS_DHE_PRIME_511B BIT(3)
#define TLS_DHE_PRIME_767B BIT(4)
#define TLS_DHE_PRIME_15 BIT(5)
#define TLS_DHE_PRIME_58B BIT(6)
#define TLS_DHE_NON_PRIME BIT(7)

void tls_connection_set_test_flags(struct tls_connection *conn, u32 flags);

int tls_get_library_version(char *buf, size_t buf_len);

void tls_connection_set_success_data(struct tls_connection *conn,
									 struct wpabuf *data);

void tls_connection_set_success_data_resumed(struct tls_connection *conn);

const struct wpabuf *tls_connection_get_success_data(struct tls_connection
													 *conn);

void tls_connection_remove_session(struct tls_connection *conn);

#endif
