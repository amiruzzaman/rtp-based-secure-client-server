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
  - [x] How to error-report?
    - [x] Return size read via a parameter, and error via return value?
      - [x] With a status enum.
  - [x] Serialize header
    - [x] Define the methods
    - [x] Test

  - [x] Serialize the rest of the packet
    - [x] Define the methods
    - [x] Test

  - [x] Deserialize a packet from a buffer
    - [x] Define the methods
    - [x] Test

### RTP session
- Since most of the header content of an RTP packet changes predictably (e.g.
  timestamp) or doesn't change at all (e.g. version), it's best to have an
  abstraction layer for this.
- We use the same notation as in the paper: RTP session.

### SIP?
- I need a way for 2 clients to exchange some info before initializing a session.
- But do I just write SIP from scratch?
- [ ] For now, let's implement a minimum version; not SIP, just send the SSRC out.

## Some getter functions
- [x] Lovely.

## UDP server with libevent
- [x] Event loop.
- [ ] Signal handling to shut down more gracefully.
- [ ] When `recvfrom` a client, create a new socket that's bound to said
  client's address and dump data there. We will save a `struct` containing the
  event handler, the socket and more information if needed.
  - [ ] We can save this in a global (or at least thread-local) buffer
    (that should be dynamically growing also).
