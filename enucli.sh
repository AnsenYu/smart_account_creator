#!/bin/bash
################################################################################
#
# Scrip Created by http://CryptoLions.io
# https://github.com/CryptoLions/scripts/
#
###############################################################################



WALLETHOST="127.0.0.1"
# NODEURL="https://jungle.eosio.cr/"
NODEURL="http://127.0.0.1:8888"
# NODEPORT="443"
NODEPORT="8000"
WALLETPORT="8900"




enucli -u $NODEURL --wallet-url http://$WALLETHOST:$WALLETPORT "$@"
