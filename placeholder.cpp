#include "placeholder.hpp"

ACTION placeholder::prepare(const name user, const uint32_t locked_until) {
  require_auth(_self);
  auto it = founder_balances.find(user.value);
  if(it == founder_balances.end()) {
    founder_balances.emplace(user, [&](auto& bal){
        bal.founder = user;
        bal.funds = asset{0, priveos_symbol};
        bal.locked_until = locked_until;
    });
  }
}

ACTION placeholder::withdraw(const name user, const asset funds) {
  require_auth(user);
  sub_balance(user, funds);
  
  action(
    permission_level{_self, "active"_n},
    priveos_token_contract,
    "transfer"_n,
    std::make_tuple(_self, user, funds, std::string("Withdrawal"))
  ).send();
}

void placeholder::transfer(const name from, const name to, const asset quantity, const std::string memo) {
  // only respond to incoming transfers
  if (from == _self || to != _self) {
    return;
  }
  eosio_assert(quantity.amount > 0, "Deposit amount must be > 0");
  eosio_assert(quantity.symbol == priveos_symbol, "Only PRIVEOS tokens allowed");

  auto bal = free_balance_singleton.get_or_default(
    freebal {
      .funds = asset{0, priveos_symbol}
    }
  );

  bal.funds += quantity;
  free_balance_singleton.set(bal, _self);
}

// EOSIO_DISPATCH(placeholder, (prepare))
extern "C" {
  [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
    if (action == "transfer"_n.value && code == placeholder::priveos_token_contract.value) {
      execute_action(eosio::name(receiver), eosio::name(code), &placeholder::transfer);
    }

    if (code == receiver) {
      switch (action) { 
        EOSIO_DISPATCH_HELPER( placeholder, (prepare) ) 
      }    
    }

    eosio_exit(0);
  }
}