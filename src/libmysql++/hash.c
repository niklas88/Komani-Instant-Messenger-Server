 int32 joaat_hash(uchar *key, size_t key_len)
 {
     uint32 hash = 0;
     size_t i;
     
     for (i = 0; i < key_len; i++) {
         hash += key[i];
         hash += (hash << 10);
         hash ^= (hash >> 6);
     }
     hash += (hash << 3);
     hash ^= (hash >> 11);
     hash += (hash << 15);
     return hash;
 }
