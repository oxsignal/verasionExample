# OSv on ARM CCA Attestation Example

This project is a Proof of Concept (PoC) demonstrating how to generate an Attestation Token within a **Realm (OSv Unikernel)** on the **ARM CCA (Confidential Compute Architecture)** environment and verify it using an external client.

## Prerequisites

* **Hardware/Emulator**: ARM v9 RME (Realm Management Extension) enabled QEMU or FVP.
* **OS**: Linux (for Client & Verifier).
* **Tools**:
    * `evcli` (Veraison CLI Tool)
    * `arc` (Attestation Report Converter)
    * `jq`, `curl`

## Architecture Flow

The overall Attestation process proceeds as follows:



1.  **Measurement Sharing (Assumption)**: The Initial Measurement (RIM) of the Realm image is assumed to be pre-shared with the Verifier via a trusted channel.
2.  **Server Start**: The Attestation Server (OSv) starts inside the Realm.
3.  **Challenge Request**: An external Client requests attestation by sending a random **Challenge** to the Realm.
4.  **Token Generation**:
    * The Realm performs an `RSI` (Realm Services Interface) call to the RMM (Realm Management Monitor).
    * The RMM generates an **Attestation Token** and **Measurement (RIM/REM)** based on the Hardware Root of Trust.
5.  **Response**: The Realm sends the generated token and measurement back to the Client.
6.  **Verification**: The Client (or Verifier) validates the token signature via the Veraison service and checks if the measurement matches the expected value.

## Build & Run

### 1. Build OSv Realm
Build the OSv image containing the `realm_server`.
*(Example command - adjust based on your build system)*
```bash
./scripts/build -j4 image=realm_app

### 2. Run Realm Server
Launch the Realm using QEMU with port forwarding enabled.

```bash
# Example: Running QEMU with port forwarding (Host 8080 -> Guest 8080)
./scripts/run.py -n -f ... --forward "tcp:8080:8080"
```

###3. Run Client
Execute the client application on the host to request the token.

```bash
./verifier_client
# Output: 'cca_example_token.cbor' file will be created.
```

## Verification
We use the Veraison service to verify the validity of the received token (cca_example_token.cbor).

Verification Script
Run the following script to perform the verification.

```bash
#!/bin/bash

# 1. Configure Veraison Test Service
export API_SERVER=[https://veraison.test.linaro.org:8443/challenge-response/v1/newSession](https://veraison.test.linaro.org:8443/challenge-response/v1/newSession)

# 2. Fetch Verification Key
echo "Fetching Verification Key..."
curl -s -N [https://veraison.test.linaro.org:8443/.well-known/veraison/verification](https://veraison.test.linaro.org:8443/.well-known/veraison/verification) \
  | jq '."ear-verification-key"' > $HOME/pkey.json

# 3. Verify Token with Veraison (relying-party mode)
# Use 'evcli' to verify the token and save the result (EAR) in JWT format.
echo "Verifying Token with Veraison..."
./evcli cca verify-as relying-party \
  --token $HOME/cca_example_token.cbor \
  | tr -d \" > $HOME/attestation_result.jwt

# 4. Verify JWT Signature (using arc tool)
# Verify that the JWT result issued by Veraison is authentic using the public key.
echo "Verifying JWT Signature..."
./arc verify --pkey $HOME/pkey.json $HOME/attestation_result.jwt

# Check exit code
if [ $? -eq 0 ]; then
    echo "✅ Verification SUCCESS!"
else
    echo "❌ Verification FAILED."
fi
```

## File Structure

`realm_server.c` : Attestation Server running inside OSv (Realm).

`verifier_client.c` : External Client that requests the token.

`cca_protocol.h` : Communication protocol definitions.
