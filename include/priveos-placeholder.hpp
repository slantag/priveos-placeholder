#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>

using namespace eosio;


CONTRACT placeholder : public contract {
   public:
      using contract::contract;
      
      constexpr static symbol priveos_symbol{"PRIVEOS", 4};
      constexpr static name priveos_token_contract{"priveostoken"};

      placeholder(name self, name code, datastream<const char*> ds) : eosio::contract(self, code, ds), founder_balances(_self, _self.value), free_balance_singleton(_self, _self.value){}
      
      TABLE free_balance {
        asset funds;
      };
      typedef singleton<"freebal"_n, free_balance> free_balance_table;
      free_balance_table free_balance_singleton;
      
      TABLE founderbal {
        name        founder;
        asset       funds;
        uint32_t    locked_until; // seconds since unix epoch
        
        uint64_t primary_key() const { return founder.value; }        
      };
      
      typedef multi_index<"founderbal"_n, founderbal> founderbal_table;
      founderbal_table founder_balances;

      ACTION prepare(
        const name user,
        const uint32_t locked_until
      );
      
      ACTION withdraw(
        const name user,
        const asset funds
      );
    
      void transfer(const name from, const name to, const asset quantity, const std::string memo);
    
    private:
      
      void add_balance(name user, asset value) {
        auto user_it = founder_balances.find(user.value);      
        eosio_assert(user_it != founder_balances.end(), "Balance table entry does not exist, call prepare first");
        founder_balances.modify(user_it, user, [&](auto& bal){
            bal.funds += value;
        });
      }
    
      void sub_balance(name user, asset value) {
        const auto& user_balance = founder_balances.get(user.value, "User has no balance");
        eosio_assert(user_balance.locked_until < now(), "Funds have not yet become unlocked");
        eosio_assert(user_balance.funds >= value, "Overdrawn balance");
        
        if(user_balance.funds == value) {
          founder_balances.erase(user_balance);
        } else {
          founder_balances.modify(user_balance, user, [&](auto& bal){
              bal.funds -= value;
          });
        }
      }    
};

