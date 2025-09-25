# To do list

## A basic 1-message UDP server-client
- [x] Status.
- Assuming I'm only dealing with Unix systems, this works.
- [x] Client can receive a small packet.
- [x] Client can receive packets of any size.
  - [x] At least, any of the size that fits in a 32-bit array. Or, the size
    ranges from 4 to 2^32 bytes.
    - [ ] But if I already know that's the maximum size, why don't I just
      pre-allocate the entire block?
  - [x] I probably have to recv with the `MSG_PEEK` flag.
  - [x] And the header should now contain the packet size.
  - [x] Now the buffer needs to be `mmap`ed or `malloc`ed.
  - For now it's nuked from the implementation.

## Send an RTP packet
### RTP header
- [x] Define the structs.
- [ ] Some convenience methods.
  - [ ] Accessors for some non-byte-aligned values.
