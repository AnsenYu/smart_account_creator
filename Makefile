CPP_IN=smart_account_creator
PK=ENU6KAV4we7bezueZ31pWf26aa8JpTe1Edp5kj298ZKBuN7c7t8D5
ENUCLI=./enucli.sh
CREATOR_ACCOUNT=twotwotwotwo
CONTRACT_ACCOUNT=newaccount12
PAYER_ACCOUNT=twotwotwotwo
EOS_CONTRACTS_DIR=/Users/al/Documents/eos/eos/build/contracts
ACCOUNT_NAME=testaccount1
ENUCLI=./enucli.sh

build:
	enumivocpp -o $(CPP_IN).wast $(CPP_IN).cpp

abi:
	enumivocpp -g $(CPP_IN).abi $(CPP_IN).cpp

all: build abi

deploy: build
	$(ENUCLI) set contract $(CONTRACT_ACCOUNT) ../smart_account_creator

setup:
	$(ENUCLI) system newaccount --stake-net "1.0000 ENU" --stake-cpu "1.0000 ENU" --buy-ram-kbytes 8000 $(CREATOR_ACCOUNT) $(CONTRACT_ACCOUNT) $(PK) $(PK)
	$(ENUCLI) set account permission $(CONTRACT_ACCOUNT) active '{"threshold": 1,"keys": [{"key": "$(PK)","weight": 1}],"accounts": [{"permission":{"actor":"$(CONTRACT_ACCOUNT)","permission":"enumivo.code"},"weight":1}]}' owner -p $(CONTRACT_ACCOUNT)

test:
	$(ENUCLI) transfer $(PAYER_ACCOUNT) $(CONTRACT_ACCOUNT) "1.0000 ENU" "$(ACCOUNT_NAME):ENU7R6HoUvevAtoLqUMSix74x9Wk4ig75tA538HaGXLFKpquKCPkH:ENU6bWFTECWtssKrHQVrkKKf68EydHNyr1ujv23KCZMFUxqwcGqC3" -p $(PAYER_ACCOUNT)@active 
	$(ENUCLI) get account $(ACCOUNT_NAME)
	
clean:
	rm -f $(CPP_IN).wast $(CPP_IN).wasm
