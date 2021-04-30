#!/usr/bin/env python3

#################################################################################
# 20201123: wrapper for weechat fish.py
#################################################################################
# See original fish.py for Copyrights, license and info:
#   https://github.com/weechat/scripts/blob/master/python/fish.py
# Does not call pack/unpack defs but re-uses relevant dh code directly:
#   - vars:  p_dh1080, q_dh1080
#   - class: DH1080Ctx
#   - defs:  bytes2int, int2bytes dh_validate_public, dh1080_b64encode
#################################################################################

import sys
# dont load weechat module 
sys.modules["weechat"] = ""
import fish as wcf

# output : my public and private key
if ((len(sys.argv) > 1) and (sys.argv[1] == "DH1080gen")):
  fish_DH1080ctx = {}
  targetl = "irc_nick"
  fish_DH1080ctx[targetl] = wcf.DH1080Ctx()
  try:
    if not 1 < fish_DH1080ctx[targetl].public < wcf.p_dh1080:
      sys.exit(1)
    if not wcf.dh_validate_public(fish_DH1080ctx[targetl].public, wcf.q_dh1080, wcf.p_dh1080):
      pass
    b64_private=wcf.dh1080_b64encode(wcf.int2bytes(fish_DH1080ctx[targetl].private))
    b64_public=wcf.dh1080_b64encode(wcf.int2bytes(fish_DH1080ctx[targetl].public))
    print(b64_private, b64_public)
    del fish_DH1080ctx[targetl]
    del b64_private
  except:
    sys.exit(1)

# input  : argv2 = my private key argv3 = someones public key
# output : shared secret
if ((len(sys.argv) > 3) and (sys.argv[1] == "DH1080comp")):
  try:
    private = wcf.bytes2int(wcf.dh1080_b64decode(sys.argv[2]))
    public = wcf.bytes2int(wcf.dh1080_b64decode(sys.argv[3]))
    if not 1 < public < wcf.p_dh1080:
      sys.exit(1)
    if not wcf.dh_validate_public(public, wcf.q_dh1080, wcf.p_dh1080):
      pass
    secret = pow(public, private, wcf.p_dh1080)
    del private
    print(wcf.dh1080_b64encode(wcf.sha256(wcf.int2bytes(secret))))
    del secret
  except:
    sys.exit(1)

#print('DEBUG: globals')
