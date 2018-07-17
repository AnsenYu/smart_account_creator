#include "smart_account_creator.hpp"

using namespace enu;
using namespace std;

class sac : public enumivo::contract {
public:
  sac(account_name self) : enumivo::contract(self) {}

  void transfer(const account_name sender, const account_name receiver) {
    const auto transfer = enumivo::unpack_action_data<enumivo::currency::transfer>();
    if (transfer.from == _self || transfer.to != _self) {
      // this is an outgoing transfer, do nothing
      return;
    }
    
    // don't do anything on transfers from our reference account
    if (transfer.from == N(ge4dknjtgqge)) {
      return;
    }
    
    /* Parse Memo
     * Memo must have format "account_name:owner_key:active_key"
     *
     */
    enumivo_assert(transfer.quantity.symbol == enumivo::string_to_symbol(4, "ENU"),
                 "Must be ENU");
    enumivo_assert(transfer.quantity.is_valid(), "Invalid token transfer");
    enumivo_assert(transfer.quantity.amount > 0, "Quantity must be positive");

    enumivo_assert(transfer.memo.length() == 120 || transfer.memo.length() == 66, "Malformed Memo (not right length)");
    const string account_string = transfer.memo.substr(0, 12);
    const account_name account_to_create =
        enumivo::string_to_name(account_string.c_str());
    enumivo_assert(transfer.memo[12] == ':' || transfer.memo[12] == '-', "Malformed Memo [12] == : or -");

    const string owner_key_str = transfer.memo.substr(13, 53);
    string active_key_str;
    if(transfer.memo[66] == ':' || transfer.memo[66] == '-') {
      // active key provided
      active_key_str = transfer.memo.substr(67, 53);
    } else {
      // active key is the same as owner
      active_key_str =  owner_key_str;
    }
    

    const abienu::public_key owner_pubkey =
        abienu::string_to_public_key(owner_key_str);
    const abienu::public_key active_pubkey =
        abienu::string_to_public_key(active_key_str);

    std::array<char, 33> owner_pubkey_char;
    copy(owner_pubkey.data.begin(), owner_pubkey.data.end(),
         owner_pubkey_char.begin());

    std::array<char, 33> active_pubkey_char;
    copy(active_pubkey.data.begin(), active_pubkey.data.end(),
         active_pubkey_char.begin());

    const auto owner_auth = authority{
        1, {{{(uint8_t)abienu::key_type::k1, owner_pubkey_char}, 1}}, {}, {}};
    const auto active_auth = authority{
        1, {{{(uint8_t)abienu::key_type::k1, active_pubkey_char}, 1}}, {}, {}};

    const auto amount = buyrambytes(4 * 1024);
    const auto cpu = asset(1000);
    const auto net = asset(1000);

    const auto fee =
        asset(std::max((transfer.quantity.amount + 119) / 200, 1000ll));
    enumivo_assert(cpu + net + amount + fee <= transfer.quantity,
                 "Not enough money");

    const auto remaining_balance = transfer.quantity - cpu - net - amount - fee;

    // create account
    INLINE_ACTION_SENDER(call::enu, newaccount)
    (N(enumivo), {{_self, N(active)}},
     {_self, account_to_create, owner_auth, active_auth});

    // buy ram
    INLINE_ACTION_SENDER(call::enu, buyram)
    (N(enumivo), {{_self, N(active)}}, {_self, account_to_create, amount});

    // delegate and transfer cpu and net
    INLINE_ACTION_SENDER(call::enu, delegatebw)
    (N(enumivo), {{_self, N(active)}}, {_self, account_to_create, net, cpu, 1});
    // fee
    INLINE_ACTION_SENDER(enu::token, transfer)
    (N(enu.token), {{_self, N(active)}},
     {_self, enumivo::string_to_name("ansenironman"), fee,
      std::string("Account creation fee")});

    if (remaining_balance.amount > 0) {
      // transfer remaining balance to new account
      INLINE_ACTION_SENDER(enu::token, transfer)
      (N(enu.token), {{_self, N(active)}},
       {_self, account_to_create, remaining_balance,
        std::string("Initial balance")});
    }
  }
};

// ENUMIVO_ABI(sac, (transfer))

#define ENUMIVO_ABI_EX(TYPE, MEMBERS)                                            \
  extern "C" {                                                                 \
  void apply(uint64_t receiver, uint64_t code, uint64_t action) {              \
    if (action == N(onerror)) {                                                \
      /* onerror is only valid if it is for the "enu" code account and       \
       * authorized by "enu"'s "active permission */                         \
      enumivo_assert(code == N(enu), "onerror action's are only valid from "   \
                                     "the \"enu\" system account");          \
    }                                                                          \
    auto self = receiver;                                                      \
    if (code == self || code == N(enu.token) || action == N(onerror)) {      \
      TYPE thiscontract(self);                                                 \
      switch (action) { ENUMIVO_API(TYPE, MEMBERS) }                             \
      /* does not allow destructor of thiscontract to run: enu_exit(0); */   \
    }                                                                          \
  }                                                                            \
  }

ENUMIVO_ABI_EX(sac, (transfer))
