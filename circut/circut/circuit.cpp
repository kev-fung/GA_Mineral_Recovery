#include "Circuit.h"


Circuit::Circuit(double vprice, double wastec, double conc_feed, double tails_feed) : valuable_price(vprice), waste_cost(wastec), iter(0), rtol(1e9)
{
	circuit_feed[0] = conc_feed;
	circuit_feed[1] = tails_feed;
}

Circuit::~Circuit()
{
}

Circuit::Circuit() : valuable_price(100.), waste_cost(500.), iter(0), rtol(1e9)
{
	circuit_feed[0] = 10;
	circuit_feed[1] = 100;
}


double Circuit::Evaluate_Circuit(std::vector<int> circuit_vector, double tolerance, int max_iterations)
{
	int max_iter = max_iterations;			// Max iterations

	// Fill up our unit_list (vector of unit objects) from circuit_vector:
	for (int i = 0; i < num_units; i++)
	{
		CUnit unit(i, circuit_vector[(2 * i) + 1], circuit_vector[(2 * i) + 2]);
		unit_list[i] = unit;

		unit_list[i].feed = CStream(circuit_feed);		// Initialise all unit feeds with circuit feed
		// printfeed(unit_list[i].feed);
		// unit_list[i].output_con_tail();				// Work out the initial tail and conc streams
	}

	// Insert conc bin and tail bin
	unit_list[num_units];
	unit_list[num_units + 1];
	unit_list[num_units].feed = CStream();				// Final concentration stream
	unit_list[num_units + 1].feed = CStream();			// Final tail stream 

	while (rtol > tolerance && iter < max_iter)							// while relative difference is more than specified tolerance
	{
		// Calculate Tail and Conc streams of all units for this time step!
		for (int i = 0; i < num_units; i++)
		{
			//	printfeed(unit_list[i].feed);
			unit_list[i].output_con_tail();
			//	printfeed(unit_list[i].conc);
			//	printfeed(unit_list[i].tail);
		}

		// Store current feed into old feed and reset feeds to zero!
		unit_list[0].store_feed();
		unit_list[0].feed.set_stream(circuit_feed);		// [10, 100]
		for (int i = 1; i < num_units + 2; i++)			// INCLUDING END STREAMS!
		{
			//		cout << "feed: ";
			//		printfeed(unit_list[i].feed);

			unit_list[i].store_feed();
			//		cout << "old feed: ";
			//		printfeed(unit_list[i].feed_old);

			unit_list[i].feed.reset_stream();			// [0, 0]
	//		cout << "new reset feed: ";
	//		printfeed(unit_list[i].feed);
		}

		// Send the Tail and Conc streams to their destination units!
		for (int i = 0; i < num_units; i++)
		{
			conc_ID = unit_list[i].conc_id;
			tail_ID = unit_list[i].tail_id;

			unit_list[i].send_streams(unit_list[conc_ID], unit_list[tail_ID]);

			//cout << "This conc " << i << ": ";
			//printfeed(unit_list[i].conc);
			//cout << "This tail " << i << ": ";
			//printfeed(unit_list[i].tail);

			//cout << "To conc " << conc_ID << ": ";
			//printfeed(unit_list[conc_ID].feed);
			//cout << "To tail " << tail_ID << ": ";
			//printfeed(unit_list[tail_ID].feed);
			//// split input feed into concentration stream and tail stream
			//unit_list[i].output_con_tail();				//unit_list[conc_ID], unit_list[tail_ID]

			//// get largest tolerance value between the valuable and waste stream
			//rel_tol = unit_list[i].rel_tol_calc();

			//if (i == 0)
			//{
			//	// add circuit feed to unit 0
			//	unit_list[0].feed.set_stream(circuit_feed);
			//	// cout << "Unit 0 feed: " << unit_list[0].feed.M[0];
			//}
			////else if ()
			//else
			//{
			//	// save feed value to other variable and 
			//	// reset feed to 0 for other units
			//	unit_list[i].feed_old = unit_list[i].feed; 
			//	unit_list[i].feed.reset_stream();

			//}
			//cout << endl;
		}

		// Work out the relative tolerances!
		max_rel_tol = -1e9;
		for (int i = 0; i < num_units; i++)
		{
			rel_tol = unit_list[i].rel_tol_calc();
			if (rel_tol > max_rel_tol)
			{
				max_rel_tol = rel_tol;
			}
		}

		cout << "Unit 0";
		printfeed(unit_list[0].feed);

		cout << "Unit 1";
		printfeed(unit_list[1].feed);

		cout << "Unit 2";
		printfeed(unit_list[2].feed);

		cout << "Conc bin stream: ";
		printfeed(unit_list[num_units].feed);

		cout << "Tails bin stream: ";
		printfeed(unit_list[num_units + 1].feed);

		rtol = max_rel_tol;
		iter++;
		/*
		system("pause");*/
	}

	cout << "Iteration: " << iter << endl;
	// Calculate fitness value based on economical value of concentration unit
	tot_valuable = unit_list[num_units].feed.M[0];
	tot_waste = unit_list[num_units].feed.M[1];

	fitness = (tot_valuable*valuable_price) - (tot_waste*waste_cost);

	return fitness;
}
