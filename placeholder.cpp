#include "placeholder.hpp"

ACTION placeholder::lock(const name user, const asset quantity, const uint32_t locked_until) {
  require_auth(_self);
  free_balance_sub(quantity);
  add_balance(user, quantity, locked_until);
  consistency_check();
}

ACTION placeholder::withdraw(const name user, const asset quantity) {
  require_auth(user);
  sub_balance(user, quantity);
  
  action(
    permission_level{_self, "active"_n},
    priveos_token_contract,
    "transfer"_n,
    std::make_tuple(_self, user, quantity, std::string("Withdrawal"))
  ).send();

  /**
    * It's not possible to see the effects of this inline action within
    * the same transaction, so we can't call consistency_check here. 
    */
}

void placeholder::delegate(const name user, const asset value) {
  require_auth(_self);
  free_balance_sub(value);
  
  auto user_it = delegations.find(user.value);      
  if(user_it == delegations.end()) {
    delegations.emplace(_self, [&](auto& bal){
        bal.user = user;
        bal.funds = value;
    });
  } else {
    delegations.modify(user_it, _self, [&](auto& bal){
        bal.funds += value;
    });
  }
  
  consistency_check();
}

ACTION placeholder::undelegate(const name user, const asset value) {
  require_auth(_self);
  free_balance_add(value);
  
  const auto& user_balance = delegations.get(user.value, "User has no balance");
  eosio_assert(user_balance.funds >= value, "Overdrawn balance");
  
  if(user_balance.funds == value) {
    delegations.erase(user_balance);
  } else {
    delegations.modify(user_balance, user, [&](auto& bal){
        bal.funds -= value;
    });
  }
  
  consistency_check();
} 

ACTION placeholder::transfer(const name from, const name to, const asset quantity, const std::string memo) {
  // only respond to incoming transfers
  if (from == _self || to != _self) {
    return;
  }
  eosio_assert(quantity.is_valid(), "Are you trying to corrupt me?");
  eosio_assert(quantity.amount > 0, "Deposit amount must be > 0");
  eosio_assert(get_code() == priveos_token_contract, "This transfer notification should only handle notifications from the priveos token contract");
  
  if(quantity.symbol == priveos_symbol) {
    free_balance_add(quantity);    
  }
  
  consistency_check();
}

extern "C" {
  [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    if (action == "transfer"_n.value && code == placeholder::priveos_token_contract.value) {
      execute_action(eosio::name(receiver), eosio::name(code), &placeholder::transfer);
    }

    if (code == receiver) {
      switch (action) { 
        EOSIO_DISPATCH_HELPER( placeholder, (lock)(withdraw)(delegate)(undelegate) ) 
      }    
    }

    eosio_exit(0);
  }
}