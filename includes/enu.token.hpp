/**
 *  @file
 *  @copyright defined in enu/LICENSE.txt
 */
#pragma once

#include <enulib/asset.hpp>
#include <enulib/enu.hpp>

#include <string>

namespace enusystem {
   class system_contract;
}

namespace enu {

   using std::string;

   class token : public enumivo::contract {
      public:
         token( account_name self ):contract(self){}

         void create( account_name issuer,
                      enumivo::asset        maximum_supply);

         void issue( account_name to, enumivo::asset quantity, string memo );

         void transfer( account_name from,
                        account_name to,
                        enumivo::asset        quantity,
                        string       memo );
      
      
         inline enumivo::asset get_supply( enumivo::symbol_name sym )const;
         
         inline enumivo::asset get_balance( account_name owner, enumivo::symbol_name sym )const;

      private:
         struct account {
            enumivo::asset    balance;

            uint64_t primary_key()const { return balance.symbol.name(); }
         };

         struct currency_stats {
            enumivo::asset          supply;
            enumivo::asset          max_supply;
            account_name   issuer;

            uint64_t primary_key()const { return supply.symbol.name(); }
         };

         typedef enumivo::multi_index<N(accounts), account> accounts;
         typedef enumivo::multi_index<N(stat), currency_stats> stats;

         void sub_balance( account_name owner, enumivo::asset value );
         void add_balance( account_name owner, enumivo::asset value, account_name ram_payer );

      public:
         struct transfer_args {
            account_name  from;
            account_name  to;
            enumivo::asset         quantity;
            string        memo;
         };
   };

   enumivo::asset token::get_supply( enumivo::symbol_name sym )const
   {
      stats statstable( _self, sym );
      const auto& st = statstable.get( sym );
      return st.supply;
   }

   enumivo::asset token::get_balance( account_name owner, enumivo::symbol_name sym )const
   {
      accounts accountstable( _self, owner );
      const auto& ac = accountstable.get( sym );
      return ac.balance;
   }

} /// namespace enu
