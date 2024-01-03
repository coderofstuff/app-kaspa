from hashlib import blake2b

def hash_init() -> blake2b:
    return blake2b(digest_size=32, key=bytes("PersonalMessageSigningHash", "ascii"))

class PersonalMessage:
    def __init__(self,
                 message: str,
                 address_type: int = 0,
                 address_index: int = 0,
                 account: int = 0x80000000,
                 ):
        self.message: bytes = bytes(message, 'utf8')    # var
        self.address_type: int = address_type           # 1 byte
        self.address_index: int = address_index         # 4 bytes
        self.account: int = account                     # 4 bytes

    def serialize(self) -> bytes:
        return b"".join([
            self.address_type.to_bytes(1, byteorder="big"),
            self.address_index.to_bytes(4, byteorder="big"),
            self.account.to_bytes(4, byteorder="big"),
            len(self.message).to_bytes(1, byteorder="big"),
            self.message
        ])

    def to_hash(self) -> bytes:
        outer_hash = hash_init()
        outer_hash.update(self.message)
        return outer_hash.digest()
