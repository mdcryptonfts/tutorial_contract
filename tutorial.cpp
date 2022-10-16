#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/symbol.hpp>
#include <string>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>



using namespace eosio;


class [[eosio::contract("codetutorial")]] codetutorial : public eosio::contract {
	

public:
	using contract::contract;
	codetutorial(name receiver, name code, datastream<const char *> ds):contract(receiver, code, ds){}


	struct [[eosio::table]] assets_s {
		uint64_t asset_id;
		name collection_name;
		name schema_name;
		int32_t template_id;
		name ram_payer;
		std::vector<asset> backed_tokens;
		std::vector<uint8_t> immutable_serialized_data;
		std::vector<uint8_t> mutable_serialized_data;
		uint64_t primary_key() const { return asset_id; }
	};

	typedef multi_index<"assets"_n, assets_s> atomics_t;




	[[eosio::on_notify("eosio.token::transfer")]] 
	void wegotpaid(name from, name to, eosio::asset quantity, std::string memo) {

		const eosio::name REAL_CONTRACT = "eosio.token"_n;

		const eosio::name ORIGINAL_CONTRACT = get_first_receiver();	

		const std::string_view waxString{"WAX"};

		const uint8_t waxdecimals = 8;

		const symbol waxsymbol(

			waxString,

			waxdecimals

		);

		const std::string_view waxdaoString{"WAXDAO"};

		const uint8_t waxdaodecimals = 8;

		const symbol waxdaosymbol(

			waxdaoString,

			waxdaodecimals

		);

		check( quantity.amount >= 100000000, "Quanity must be greater than 1 WAX" );

		check( REAL_CONTRACT == ORIGINAL_CONTRACT, "You tryna get over on us, bro?" );

		if( from == get_self() || to != get_self() ){ return; }

		check( quantity.symbol == waxsymbol, "Symbol is not what we were expecting" );

		//emplace info into table

		bal_table bals( get_self(), get_self().value );

		auto itr = bals.find( from.value );

		if( itr != bals.end() ){

			//modify this user's entry

			bals.modify( itr, same_payer, [&](auto &row) {

				row.balance += quantity;

			});

		} else {

			bals.emplace( get_self(), [&](auto &row) {

				row.user = from.value;

				row.balance = quantity;

			});

		}

		action(permission_level{eosio::name("codetutorial"), "active"_n}, "mdcryptonfts"_n,"transfer"_n,std::tuple{ eosio::name("codetutorial"), from, asset(100000000, waxdaosymbol), std::string("We just reacted son")}).send();

	}//end of on_notify



	[[eosio::on_notify("atomicassets::transfer")]] 
	void listen(name from, name to, std::vector<uint64_t> &ids, std::string memo) {

		const eosio::name REAL_CONTRACT = "atomicassets"_n;

		const eosio::name ORIGINAL_CONTRACT = get_first_receiver();	

		check( REAL_CONTRACT == ORIGINAL_CONTRACT, "You tryna get over on us, bro?" );

		if( from == get_self() || to != get_self() ){ return; }

		check( ids.size() >= 1, "You must send at least 1 NFT" );

		atomics_t assetidtbl( "atomicassets"_n, get_self().value );

		for( auto THIS_ASSET: ids ){

			auto asset_itr = assetidtbl.find(THIS_ASSET);

			check(asset_itr != assetidtbl.end(), "Could not locate this asset");

			burn_asset(THIS_ASSET);	

		}
	



	}//end atomicassets transfer





	[[eosio::action]]
	void addmessage( name user, std::string message )
	{
		require_auth( user );

		check( message.length() >= 10 && message.length() <= 100, "Your message was either too long or too short" );

		//emplace info into table

		msg_table msgs( get_self(), get_self().value );

		msgs.emplace( user, [&](auto &row) {

			row.ID = msgs.available_primary_key();

			row.message = message;

		});

	} //end addmessage


	[[eosio::action]]
	void adduserrec( name user )
	{
		require_auth( get_self() );

		//emplace info into table

		balsss_table usertable( get_self(), get_self().value );

		usertable.emplace( get_self(), [&](auto &row) {

			row.ID = usertable.available_primary_key();

			row.username = user;

			row.total_records = 0;

		});

	} //end addmessage



	[[eosio::action]]
	void countrecords( name user )
	{
		require_auth( get_self() );

		//emplace info into table

		balsss_table usertable( get_self(), get_self().value );

		auto itr = usertable.find(2);

		check( itr != usertable.end(), "Row 2 was not found" );

		auto users_secondary = usertable.get_index<"username"_n>();

		auto users_low_itr = users_secondary.lower_bound("mikedcrypto5"_n.value);

		auto users_up_itr = users_secondary.upper_bound("mikedcrypto5"_n.value);

		int foundCount = 0;

		for( auto found_itr = users_low_itr; found_itr != users_up_itr; found_itr ++ ){

			foundCount ++;

		}


		usertable.modify( itr, get_self(), [&](auto &row) {

			row.total_records = foundCount;

		});

	} //end addmessage


	

//END OF PUBLIC

private:

    //burn an asset
    void burn_asset(uint64_t assetID){
      action(
	      permission_level{get_self(), "active"_n},
	      "atomicassets"_n,
	      "burnasset"_n,
	      std::tuple{ get_self(), assetID }
  	  ).send();
  	}


	struct [[eosio::table]] messages {

		uint64_t ID;

		std::string message; 

		uint64_t primary_key() const { return ID; }

	};

	using msg_table = eosio::multi_index<"messages"_n, messages
	>;


	struct [[eosio::table]] balances {

		uint64_t user;

		eosio::asset balance;

		uint64_t primary_key() const { return user; }

	};

	using bal_table = eosio::multi_index<"balances"_n, balances
	>;


	struct [[eosio::table]] balancesss {

		uint64_t ID;

		eosio::name username;

		eosio::asset balance;

		uint64_t total_records;

		uint64_t primary_key() const { return ID; }

		uint64_t second_key() const { return username.value; }

	};

	using balsss_table = eosio::multi_index<"balancesss"_n, balancesss,
	eosio::indexed_by<"username"_n, eosio::const_mem_fun<balancesss, uint64_t, &balancesss::second_key>>
	>;



//END OF PRIVATE


	};
