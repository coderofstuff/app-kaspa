from pathlib import Path

from secp256k1 import PublicKey

from application_client.kaspa_transaction import Transaction

ROOT_SCREENSHOT_PATH = Path(__file__).parent.resolve()

# Check if a signature of a given message is valid
def check_signature_validity(public_key: bytes, signature: bytes, sighash: bytes, transaction: Transaction) -> bool:
    pkey = PublicKey(public_key, True)
    
    return pkey.schnorr_verify(sighash, signature, None, True)
