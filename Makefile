CPP_IN=placeholder
PUBLIC_KEY=EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
CONTRACT_ACCOUNT=priveosrules
EOS_CONTRACTS_DIR=/Users/al/Projects/eos/eos/build/contracts
CLEOS=cleos

build:
	 eosio-cpp -abigen $(CPP_IN).cpp -o $(CPP_IN).wasm 
	
deploy: build
	$(CLEOS) set contract $(CONTRACT_ACCOUNT) . $(CPP_IN).wasm $(CPP_IN).abi

init:
	make system
	make setup
	make deploy


create_wallet:
	$(CLEOS) wallet create --file=wallet-password.txt

import_keys:
	$(CLEOS) wallet import --name=eosio --private-key=$(PRIVATE_KEY)
	$(CLEOS) wallet import --name=eosio --private-key=$(ALICE_PRIVATE_KEY)
	$(CLEOS) wallet import --name=eosio --private-key=$(BOB_PRIVATE_KEY)

unlock:
	$(CLEOS) wallet unlock --password $(WALLET_PASSWORD)
		
system:
	$(CLEOS) create account eosio eosio.token $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) create account eosio eosio.msig $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) create account eosio eosio.bpay $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) create account eosio eosio.names $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) create account eosio eosio.ram $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) create account eosio eosio.ramfee $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) create account eosio eosio.saving $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) create account eosio eosio.stake $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) create account eosio eosio.vpay $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) set contract eosio.token $(EOS_CONTRACTS_DIR)/eosio.token -p eosio.token
	$(CLEOS) set contract eosio.msig $(EOS_CONTRACTS_DIR)/eosio.msig -p eosio.msig
	$(CLEOS) push action eosio.token create '["eosio", "10000000000.0000 EOS",0,0,0]' -p eosio.token
	$(CLEOS) push action eosio.token issue '["eosio","1000000000.0000 EOS", "issue"]' -p eosio
	$(CLEOS) set contract eosio $(EOS_CONTRACTS_DIR)/eosio.system -p eosio
	cleos push action eosio setpriv '["eosio.msig", 1]' -p eosio


setup:
	$(CLEOS) system newaccount --stake-net "1.0000 EOS" --stake-cpu "1.0000 EOS" --buy-ram-kbytes 8000 eosio $(CONTRACT_ACCOUNT) $(PUBLIC_KEY) $(PUBLIC_KEY)
	$(CLEOS) set account permission $(CONTRACT_ACCOUNT) active '{"threshold": 1,"keys": [{"key": "$(PUBLIC_KEY)","weight": 1}],"accounts": [{"permission":{"actor":"$(CONTRACT_ACCOUNT)","permission":"eosio.code"},"weight":1}]}' owner -p $(CONTRACT_ACCOUNT)

		
clean:
	rm -f $(CPP_IN).wast $(CPP_IN).wasm $(CPP_IN).abi 
