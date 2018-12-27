CLEOS=cleos
PUBLIC_KEY=EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
EOS_CONTRACTS_DIR=/Users/al/Projects/eos/eos/build/contracts
CONTRACT_ACCOUNT=priveosrules

# cleos create account eosio eosio.token $PUBLIC_KEY $PUBLIC_KEY
# cleos create account eosio eosio.msig $PUBLIC_KEY $PUBLIC_KEY
# cleos create account eosio eosio.bpay $PUBLIC_KEY $PUBLIC_KEY
# cleos create account eosio eosio.names $PUBLIC_KEY $PUBLIC_KEY
# cleos create account eosio eosio.ram $PUBLIC_KEY $PUBLIC_KEY
# cleos create account eosio eosio.ramfee $PUBLIC_KEY $PUBLIC_KEY
# cleos create account eosio eosio.saving $PUBLIC_KEY $PUBLIC_KEY
# cleos create account eosio eosio.stake $PUBLIC_KEY $PUBLIC_KEY
# cleos create account eosio eosio.vpay $PUBLIC_KEY $PUBLIC_KEY
# cleos set contract eosio.token $EOS_CONTRACTS_DIR/eosio.token -p eosio.token
# cleos set contract eosio.msig $EOS_CONTRACTS_DIR/eosio.msig -p eosio.msig
# cleos push action eosio.token create '["eosio", "10000000000.0000 EOS",0,0,0]' -p eosio.token
# cleos push action eosio.token issue '["eosio","1000000000.0000 EOS", "issue"]' -p eosio
# cleos set contract eosio $EOS_CONTRACTS_DIR/eosio.system -p eosio

# cleos system newaccount --stake-net "1.0000 EOS" --stake-cpu "1.0000 EOS" --buy-ram-kbytes 8000 eosio $CONTRACT_ACCOUNT $PUBLIC_KEY $PUBLIC_KEY
cleos set account permission $CONTRACT_ACCOUNT active "{\"threshold\": 1,\"keys\": [{\"key\": \"$PUBLIC_KEY\",\"weight\": 1}],\"accounts\": [{\"permission\":{\"actor\":\"$CONTRACT_ACCOUNT\",\"permission\":\"eosio.code\"},\"weight\":1}]}" owner -p $CONTRACT_ACCOUNT
