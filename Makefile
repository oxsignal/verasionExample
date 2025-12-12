# Root Makefile

all: server client

server:
	$(MAKE) -C realm_server

client:
	$(MAKE) -C verifier_client

# OSv용 Shared Object 빌드 (서버만)
osv:
	$(MAKE) -C realm_server osv

clean:
	$(MAKE) -C realm_server clean
	$(MAKE) -C verifier_client clean
	rm cca_example_token.cbor
	rm cca_example_measurement

