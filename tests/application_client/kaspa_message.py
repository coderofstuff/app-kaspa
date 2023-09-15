from hashlib import blake2b

def hash_init() -> blake2b:
    return blake2b(digest_size=32, key=bytes("PersonalMessageSigningHash", "ascii"))

class PersonalMessage:
    def __init__(self,
                 address_type: int,
                 address_index: int,
                 message: str):
        self.address_type: int = address_type           # 1 byte
        self.address_index:int  = address_index         # 4 bytes
        self.message: bytes = bytes(message, 'utf8')    # var

    def serialize(self) -> bytes:
        return b"".join([
            self.address_type.to_bytes(1, byteorder="big"),
            self.address_index.to_bytes(4, byteorder="big"),
            len(self.message).to_bytes(1, byteorder="big"),
            self.message
        ])

    def to_hash(self) -> bytes:
        outer_hash = hash_init()
        outer_hash.update(self.message)
        return outer_hash.digest()
