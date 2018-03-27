#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <unordered_map>

using namespace std;

#define DONE 10
#define IOC 11
#define GFD 12

#define CANCEL  0
#define PRINT   1
#define BUY     2
#define SELL    3
#define MODIFY  4

#define COMMAND_SIZE 6
#define COMMAND_INDEX 0

#define ORDER_ID_INDEX_IN_BUY 4
#define ORDER_TYPE_INDEX_IN_BUY 1
#define ORDER_PRICE_INDEX_IN_BUY 2
#define ORDER_QUANTITY_INDEX_IN_BUY 3

#define ORDER_ID_INDEX_IN_SELL 4
#define ORDER_TYPE_INDEX_IN_SELL 1
#define ORDER_PRICE_INDEX_IN_SELL 2
#define ORDER_QUANTITY_INDEX_IN_SELL 3

#define ORDER_ID_IN_CANCEL 1

#define ORDER_ID_IN_MODIFY 1
#define ORDER_TYPE_IN_MODIFY 2
#define ORDER_PRICE_IN_MODIFY 3
#define ORDER_QUANTITY_IN_MODIFY 4

struct Order {
	string id;
	int type;
	int price;
	int quantity;
};

struct Action {
	int action_type;
	Order action_input;
};

struct Trade {
	Order trade_1;
	Order trade_2;
};


namespace Data {
	vector<Trade> trades;
	map<int, list<Order> > order_index_buy;
	map<int, list<Order> > order_index_sell;
	map<string, Order> order_buy;
	map<string, Order> order_sell;

	int in_order_map(map<string, Order> &map_order, Order order) {
		return map_order.count(order.id);
	}
	int merge_order(map<string, Order> &map_order, Order order) {
		if (map_order.count(order.id)) {
			Order order_already_in = map_order[order.id];
			map_order[order.id] = order;
		}
		else
			map_order.insert(pair<string, Order>(order.id, order));
		return 0;
	}
	int add_order_index(map<int, list<Order> > &map_list_order, Order order) {
		if (map_list_order.count(order.price)) {
			map_list_order[order.price].push_back(order);
		}
		else {
			list<Order> order_list;
			order_list.push_back(order);
			map_list_order.insert(pair<int, list<Order> >(order.price, order_list));
		}
		return 0;
	}
	int add_buy(Order order) {
		if (!in_order_map(order_buy, order)) {
			merge_order(order_buy, order);
			if (order.type == GFD)
				add_order_index(order_index_buy, order);
			return 0;
		}
		return 1;
	}

	int add_sell(Order order) {
		if (!in_order_map(order_sell, order)) {
			merge_order(order_sell, order);
			if (order.type == GFD)
				add_order_index(order_index_sell, order);
			return 0;
		}
		return 1;
	}
	bool compare_up(int a, int b) {
		return a<b;
	}
	bool compare_down(int a, int b) {
		return a>b;
	}

	Trade make_trade(Order trade_1, Order trade_2) {
		cout << "TRADE " << trade_1.id << " " << trade_1.price << " " << trade_1.quantity << " " << trade_2.id << " " << trade_2.price << " " << trade_2.quantity << endl;
		struct Trade result = {
			trade_1,
			trade_2
		};
		return result;
	}

	Order do_buy(Order &order) {
		vector<int> prices;
		for (map< int, list<Order> >::iterator orders = order_index_sell.begin(); orders != order_index_sell.end(); ++orders) {
			prices.push_back(orders->first);
		}
		sort(prices.begin(), prices.end(), compare_down);
		for (vector<int>::iterator price = prices.begin(); price != prices.end(); ++price) {
			for (list<Order>::iterator sell_order = order_index_sell[*price].begin(); sell_order != order_index_sell[*price].end(); ) {
				if ((sell_order->quantity > order.quantity) && (sell_order->price <= order.price) && order.quantity > 0) {
					// frash order list
					sell_order->quantity -= order.quantity;
					// frash order map
					order_sell[sell_order->id].quantity = sell_order->quantity;
					struct Order s_order = {
						sell_order->id,
						sell_order->type,
						sell_order->price,
						order.quantity
					};
					//frash trades list
					trades.push_back(make_trade(s_order, order));
					order.quantity = 0;
				}
				if ((sell_order->quantity <= order.quantity) && (sell_order->price <= order.price)) {
					struct Order s_order = {
						order.id,
						order.type,
						order.price,
						sell_order->quantity
					};
					//frash trades list
					trades.push_back(make_trade(*sell_order, s_order));
					// frash order status
					order.quantity -= sell_order->quantity;
					// frash order list
					sell_order->quantity = 0;
					// frash order map
					order_sell[sell_order->id].quantity = sell_order->quantity;
					order_sell[sell_order->id].type = DONE;
				}
				if (sell_order->quantity == 0)
					sell_order = order_index_sell[*price].erase(sell_order);
				else
					++sell_order;
				if (order.quantity == 0) {
					// frash order status
					order.type = DONE;
					return order;
				}
			}
		}
		return order;
	}

	Order do_sell(Order order) {
		vector<int> prices;
		for (map< int, list<Order> >::iterator orders = order_index_buy.begin(); orders != order_index_buy.end(); ++orders) {
			prices.push_back(orders->first);
		}
		sort(prices.begin(), prices.end(), compare_down);
		for (vector<int>::iterator price = prices.begin(); price != prices.end(); ++price) {
			for (list<Order>::iterator buy_order = order_index_buy[*price].begin(); buy_order != order_index_buy[*price].end(); ) {
				if ((buy_order->quantity > order.quantity) && (buy_order->price >= order.price) && order.quantity > 0) {
					// frash order list
					buy_order->quantity -= order.quantity;
					// frash order map
					order_buy[buy_order->id].quantity = buy_order->quantity;
					struct Order b_order = {
						buy_order->id,
						buy_order->type,
						buy_order->price,
						order.quantity
					};
					//frash trades list
					trades.push_back(make_trade(b_order, order));
					order.quantity = 0;
				}
				if ((buy_order->quantity <= order.quantity) && (buy_order->price >= order.price)) {
					struct Order b_order = {
						order.id,
						order.type,
						order.price,
						buy_order->quantity
					};
					//frash trades list
					trades.push_back(make_trade(*buy_order, b_order));
					// frash order status
					order.quantity -= buy_order->quantity;
					// frash order list
					buy_order->quantity = 0;
					// frash order map
					order_buy[buy_order->id].quantity = buy_order->quantity;
					order_buy[buy_order->id].type = DONE;
				}
				if (buy_order->quantity == 0)
					buy_order = order_index_buy[*price].erase(buy_order);
				else
					++buy_order;
				if (order.quantity == 0) {
					// frash order status
					order.type = DONE;
					return order;
				}
			}
		}
		return order;
	}

	int do_modify(Order order) {
		if (in_order_map(order_buy, order)) {
			Order old_order = order_buy[order.id];
			// remove from list
			for (list<Order>::iterator buy_order = order_index_buy[old_order.price].begin(); buy_order != order_index_buy[old_order.price].end(); ) {
				if (buy_order->id == old_order.id)
					buy_order = order_index_buy[buy_order->price].erase(buy_order);
				else
					++buy_order;
			}
			// remove form map
			order_buy.erase(order.id);
			return 0;
		}
		if (in_order_map(order_sell, order)) {
			Order old_order = order_sell[order.id];
			// remove from list
			for (list<Order>::iterator sell_order = order_index_sell[old_order.price].begin(); sell_order != order_index_sell[old_order.price].end(); ) {
				if (sell_order->id == old_order.id)
					sell_order = order_index_buy[sell_order->price].erase(sell_order);
				else
					++sell_order;
			}
			// remove form map
			order_sell.erase(order.id);
			return 0;
		}
		return 0;
	}

	int do_cancel(Order order) {
		if (in_order_map(order_buy, order)) {
			Order old_order = order_buy[order.id];
			// remove from list
			for (list<Order>::iterator buy_order = order_index_buy[old_order.price].begin(); buy_order != order_index_buy[old_order.price].end(); ) {
				if (buy_order->id == old_order.id)
					buy_order = order_index_buy[old_order.price].erase(buy_order);
				else
					++buy_order;
			}
			order_buy[order.id].quantity = 0;
			order_buy[order.id].type = DONE;
		}
		if (in_order_map(order_sell, order)) {
			Order old_order = order_sell[order.id];
			// remove from list
			for (list<Order>::iterator sell_order = order_index_sell[old_order.price].begin(); sell_order != order_index_sell[old_order.price].end(); ) {
				if (sell_order->id == old_order.id)
					sell_order = order_index_sell[sell_order->price].erase(sell_order);
				else
					++sell_order;
			}
			order_sell[order.id].quantity = 0;
			order_sell[order.id].type = DONE;
		}
		return 0;
	}

	int do_print(Order) {
		cout << "SELL:" << endl;
		for (map< int, list<Order> >::iterator orders = order_index_sell.begin(); orders != order_index_sell.end(); ++orders) {
			int all_quantity = 0;
			for (list<Order>::iterator sell_order = orders->second.begin(); sell_order != orders->second.end(); sell_order++) {
				all_quantity += sell_order->quantity;
			}
			if (all_quantity > 0)
				cout << orders->first << " " << all_quantity << endl;
		}
		cout << "BUY:" << endl;
		for (map< int, list<Order> >::reverse_iterator orders = order_index_buy.rbegin(); orders != order_index_buy.rend(); ++orders) {
			int all_quantity = 0;
			for (list<Order>::iterator buy_order = orders->second.begin(); buy_order != orders->second.end(); buy_order++) {
				all_quantity += buy_order->quantity;
			}
			if (all_quantity > 0)
				cout << orders->first << " " << all_quantity << endl;
		}
		return 0;
	}
};

bool is_digits(const std::string &str)
{
    return all_of(str.begin(), str.end(), ::isdigit);
}

Action buy_action_build(vector<string> words) {
	Action result;
	result.action_type = BUY;
	struct Order order = {
		words[ORDER_ID_INDEX_IN_BUY],
		0,
		is_digits(words[ORDER_PRICE_INDEX_IN_BUY]) ? stoi(words[ORDER_PRICE_INDEX_IN_BUY]) : 0,
		is_digits(words[ORDER_QUANTITY_INDEX_IN_BUY]) ? stoi(words[ORDER_QUANTITY_INDEX_IN_BUY]) : 0
	};
	if (words[ORDER_TYPE_INDEX_IN_BUY] == "IOC")
		order.type = IOC;
	if (words[ORDER_TYPE_INDEX_IN_BUY] == "GFD")
		order.type = GFD;
	result.action_input = order;
	return result;
}

Action sell_action_build(vector<string> words) {
	Action result;
	result.action_type = SELL;
	struct Order order = {
		words[ORDER_ID_INDEX_IN_SELL],
		0,
		is_digits(words[ORDER_PRICE_INDEX_IN_SELL]) ? stoi(words[ORDER_PRICE_INDEX_IN_SELL]) : 0,
		is_digits(words[ORDER_QUANTITY_INDEX_IN_SELL]) ? stoi(words[ORDER_QUANTITY_INDEX_IN_SELL]) : 0
	};
	if (words[ORDER_TYPE_INDEX_IN_SELL] == "IOC")
		order.type = IOC;
	if (words[ORDER_TYPE_INDEX_IN_SELL] == "GFD")
		order.type = GFD;
	result.action_input = order;
	return result;
}

Action cancel_action_build(vector<string> words) {
	Action result;
	result.action_type = CANCEL;
	struct Order order = {
		words[ORDER_ID_IN_CANCEL],
		0,
		0,
		0,
	};
	result.action_input = order;
	return result;
}

Action modify_action_build(vector<string> words) {
	Action result;
	result.action_type = MODIFY;
	struct Order order = {
		words[ORDER_ID_IN_MODIFY],
		0,
		is_digits(words[ORDER_PRICE_IN_MODIFY]) ? stoi(words[ORDER_PRICE_IN_MODIFY]) : 0,
		is_digits(words[ORDER_QUANTITY_IN_MODIFY]) ? stoi(words[ORDER_QUANTITY_IN_MODIFY]) : 0
	};
	if (words[ORDER_TYPE_IN_MODIFY] == "BUY")
		order.type = BUY;
	if (words[ORDER_TYPE_IN_MODIFY] == "SELL")
		order.type = SELL;
	result.action_input = order;
	return result;
}

Action print_action_build(vector<string> words) {
	Action result;
	result.action_type = PRINT;
	return result;
}

Action input_analysis(std::string section) {
	Action result;
	istringstream iss(section), end;
	vector<string> words(COMMAND_SIZE);
	copy(istream_iterator<string>(iss), istream_iterator<string>(end), words.begin());
	if (words[COMMAND_INDEX] == "BUY")
		result = buy_action_build(words);
	if (words[COMMAND_INDEX] == "SELL")
		result = sell_action_build(words);
	if (words[COMMAND_INDEX] == "CANCEL")
		result = cancel_action_build(words);
	if (words[COMMAND_INDEX] == "MODIFY")
		result = modify_action_build(words);
	if (words[COMMAND_INDEX] == "PRINT")
		result = print_action_build(words);
	return result;
}

int buy_process(Action action) {
	if (!Data::in_order_map(Data::order_buy, action.action_input) && !Data::in_order_map(Data::order_sell, action.action_input)) {
		struct Order order = Data::do_buy(action.action_input);
		if (order.type != IOC)
			Data::add_buy(order);
	}
	return 0;
}

int sell_process(Action action) {
	if (!Data::in_order_map(Data::order_sell, action.action_input) && !Data::in_order_map(Data::order_buy, action.action_input)) {
		struct Order order = Data::do_sell(action.action_input);
		if (order.type != IOC)
			Data::add_sell(order);
	}
	return 0;
}

int cancel_process(Action action) {
	Data::do_cancel(action.action_input);
	return 0;
}

int modify_process(Action action) {
	Data::do_modify(action.action_input);
	if (action.action_input.type == BUY) {
		struct Order order = Data::do_buy(action.action_input);
		if (order.type == BUY) {
			if (order.price != 0)
				order.type = GFD;
			else
				order.type = DONE;
			Data::add_buy(order);
		}

	}
	if (action.action_input.type == SELL) {
		struct Order order = Data::do_sell(action.action_input);
		if (order.type == SELL) {
			if (order.price != 0)
				order.type = GFD;
			else
				order.type = DONE;
			Data::add_sell(order);
		}
	}
	return 0;
}

int print_process(Action action) {
	Data::do_print(action.action_input);
	return 0;
}

int action_process(Action action) {
	int result = 0;
	if (action.action_type == BUY)
		result = buy_process(action);
	if (action.action_type == SELL)
		result = sell_process(action);
	if (action.action_type == CANCEL)
		result = cancel_process(action);
	if (action.action_type == MODIFY)
		result = modify_process(action);
	if (action.action_type == PRINT)
		result = print_process(action);
	return result;
}

int main() {
	/* Enter your code here. Read input from STDIN. Print output to STDOUT */
	
	string section;
	while (getline(cin, section)) {
		Action ac = input_analysis(section);
		int result = action_process(ac);
	}
	return 0;
}
