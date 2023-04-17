using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Patience.Output;
using Patience.Tests;

namespace Patience.Rapports
{
	public class Rapport
	{
		public static List<IRapporter> Rapporters = new();


		// add rapporters on program init
		static Rapport()
		{
			Rapporters.Add(new TextRapport());
		}



		public static void LogTest(Test test)
		{
			foreach (IRapporter r in Rapporters)
				r.LogTest(test);
		}



		public static void LogTestSet(TestSet testSet)
		{
			foreach (IRapporter r in Rapporters)
				r.LogTestSet(testSet);
		}



		// finalize logging
		public static void WriteRapport(TestSet rootSet)
		{
			foreach (IRapporter r in Rapporters)
				r.WriteRapport(rootSet);
		}
	}
}
