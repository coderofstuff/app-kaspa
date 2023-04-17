from pathlib import Path
from hashlib import sha256
from sha3 import keccak_256

from ecdsa.curves import SECP256k1
from ecdsa.keys import VerifyingKey
from ecdsa.util import sigdecode_der

from application_client.kaspa_transaction import Transaction


ROOT_SCREENSHOT_PATH = Path(__file__).parent.resolve()


# Check if a signature of a given message is valid
def check_signature_validity(public_key: bytes, signature: bytes, transaction: Transaction) -> bool:
    # FIXME: Implement this unit test when signing works
    return True
