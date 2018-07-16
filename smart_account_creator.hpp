#include <algorithm>
#include <cmath>
#include <enulib/asset.hpp>
#include <enulib/currency.hpp>
#include <enulib/enu.hpp>
#include <enulib/public_key.hpp>
#include "includes/enu.token.hpp"
#include "includes/exchange_state.cpp"
#include "includes/exchange_state.hpp"
#include "includes/abieos_numeric.hpp"


namespace enu {

struct permission_level_weight {
  permission_level permission;
  weight_type weight;

  // explicit serialization macro is not necessary, used here only to improve
  // compilation time
  ENULIB_SERIALIZE(permission_level_weight, (permission)(weight))
};

struct key_weight {
  enu::public_key key;
  weight_type weight;

  // explicit serialization macro is not necessary, used here only to improve
  // compilation time
  ENULIB_SERIALIZE(key_weight, (key)(weight))
};

struct wait_weight {
  uint32_t wait_sec;
  weight_type weight;

  // explicit serialization macro is not necessary, used here only to improve
  // compilation time
  ENULIB_SERIALIZE(wait_weight, (wait_sec)(weight))
};

struct authority {
  uint32_t threshold;
  vector<key_weight> keys;
  vector<permission_level_weight> accounts;
  vector<wait_weight> waits;

  // explicit serialization macro is not necessary, used here only to improve
  // compilation time
  ENULIB_SERIALIZE(authority, (threshold)(keys)(accounts)(waits))
};

struct call {
  struct enu {
    void newaccount(account_name creator, account_name name, authority owner,
                    authority active);
    void delegatebw(account_name from, account_name receiver,
                    asset stake_net_quantity, asset stake_cpu_quantity,
                    bool transfer);
    void buyram(account_name buyer, account_name receiver, asset tokens);
  };
};
asset buyrambytes(uint32_t bytes) {
  rammarket market(N(enu), N(enu));
  auto itr = market.find(S(4, RAMCORE));
  enumivo_assert(itr != market.end(), "RAMCORE market not found");
  auto tmp = *itr;
  return tmp.convert(asset(bytes, S(0, RAM)), CORE_SYMBOL);
}
}
