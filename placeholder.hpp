#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include "eosio.token.hpp"

using namespace eosio;


CONTRACT placeholder : public contract {
   public:
      using contract::contract;
      
      static constexpr symbol priveos_symbol{"PRIVEOS", 4};
      static constexpr name priveos_token_contract{"priveostoken"};

      placeholder(name self, name code, datastream<const char*> ds) : eosio::contract(self, code, ds), founder_balances(_self, _self.value), free_balance_singleton(_self, _self.value), delegations(_self, _self.value){}
      
      TABLE freebal {
        asset funds;
      };
      typedef singleton<"freebal"_n, freebal> free_balance_table;
      free_balance_table free_balance_singleton;
      
      TABLE founderbal {
        name        founder;
        asset       funds;
        uint32_t    locked_until; // seconds since unix epoch
        
        uint64_t primary_key() const { return founder.value; }        
      };
      
      typedef multi_index<"founderbal"_n, founderbal> founderbal_table;
      founderbal_table founder_balances;
      
      TABLE delegation {
        name        user;
        asset       funds;
        
        uint64_t primary_key() const { return user.value; }        
      };
      typedef multi_index<"delegation"_n, delegation> delegations_table;
      delegations_table delegations;
       
      ACTION lock(
        const name user,
        const asset quantity,
        const uint32_t locked_until
      );
      
      ACTION withdraw(
        const name user,
        const asset quantity
      );
    
      void transfer(const name from, const name to, const asset quantity, const std::string memo);
      
      ACTION delegate(
        const name user, 
        const asset value
      );
    
      ACTION undelegate(
        const name user, 
        const asset value
      );
          
    private:
      
      void add_balance(const name user, const asset value, const uint32_t locked_until) {
        eosio_assert(value.symbol == priveos_symbol, "Only PRIVEOS tokens allowed");

        auto user_it = founder_balances.find(user.value);      
        if(user_it == founder_balances.end()) {
          founder_balances.emplace(_self, [&](auto& bal){
              bal.founder = user;
              bal.funds = value;
              bal.locked_until = locked_until;
          });
        } else {
          const auto bal = *user_it;
          eosio_assert(bal.locked_until == locked_until, "The locked_until values don't match");
          founder_balances.modify(user_it, _self, [&](auto& bal){
              bal.funds += value;
          });
        }
      }
    
      void sub_balance(const name user, const asset value) {
        eosio_assert(value.symbol == priveos_symbol, "Only PRIVEOS tokens allowed");
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
      
      void free_balance_add(const asset quantity) {
        eosio_assert(quantity.symbol == priveos_symbol, "Only PRIVEOS tokens allowed");
        auto bal = free_balance_singleton.get_or_default(
          freebal {
            .funds = asset{0, priveos_symbol}
          }
        );

        bal.funds += quantity;
        free_balance_singleton.set(bal, _self);
      } 
      
      void free_balance_sub(const asset quantity) {
        eosio_assert(quantity.symbol == priveos_symbol, "Only PRIVEOS tokens allowed");
        auto bal = free_balance_singleton.get();
        eosio_assert(bal.funds >= quantity, "Trying to overdraw free balance");
        bal.funds -= quantity;
        free_balance_singleton.set(bal, _self);
      }  
      
      void consistency_check() {
        const auto total_balance = eosio::token::get_balance(priveos_token_contract, get_self(), priveos_symbol.code());
        const auto free_balance = free_balance_singleton.get().funds;
        
        asset founders{0, priveos_symbol};
        for(const auto& x: founder_balances) {
          founders += x.funds;
        }
        
        asset delegated{0, priveos_symbol};
        for(const auto& x: delegations) {
          delegated += x.funds;
        }
        
        /**
          * Make sure the tracked funds match up with the total amount of 
          * privEOS tokens that are deposited. The idea is that the privEOS tokens
          * that are deposited in this DAC contract can belong to either category:
          * 1) Founder Balances
          *    These funds are permanent tokens that belong to
          *    the founders that are locked in until a date in the future. These
          *    tokens count towards the owner's percentage in DAC revenue payout
          *    calculations.
          * 2) Delegations
          *    Tokens of the non-permanent/delegated form that are
          *    delegated to somebody like the DAC service company or BPs. These
          *    tokens count towards the delegatee's percentage in DAC revenue
          *    payout calculations. Unlike permanent tokens, the delegatee only 
          *    has a claim to the revenue payout for as long as those tokens are
          *    delegated to them. The DAC can undelegate any time when certain
          *    conditions are met (non-compliance of BP etc.).
          * 3) Free balance
          *    Tokens that have been deposited into the contract but have
          *    not yet been assigned to any of the above categories.
          *    In the very beginning, the DAC will have a free balance of 
          *    600 PRIVEOS reserved to be delegated to the participating BPs.
          */
        eosio_assert(free_balance + founders + delegated == total_balance, "Inconsistent balances");
      }
      
      
};

