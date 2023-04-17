using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Patience.Tests;

namespace Patience.Rapports
{
	public interface IRapporter
	{
		public void LogTest(Test test);
		public void LogTestSet(TestSet testSet);
		public void WriteRapport(TestSet rootSet);
	}
}
