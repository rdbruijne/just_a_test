using System.Linq.Expressions;
using Patience.Tests;

namespace Patience.Comparing
{
	internal static class Compare
	{
		private static List<ICompare> Comparers = new();
		private static readonly DefaultCompare Default = new();



		static Compare()
		{
			Comparers.Clear();
			// TODO: add specialized comparers
		}



		public static bool Files(string outPath, string refPath, ref List<string> extraInfo)
		{
			bool outExists = File.Exists(outPath);
			if (!outExists)
				extraInfo.Add($"Output file does not exist: {Utility.SanitizePath(outPath)}");

			bool refExists = File.Exists(refPath);
			if (!refExists)
				extraInfo.Add($"Reference file does not exist: {Utility.SanitizePath(refPath)}");

			if (!outExists || !refExists)
				return false;

			string fileExt = Path.GetExtension(outPath).ToLower();
			foreach (ICompare c in Comparers)
			{
				if(c.SupportsType(fileExt))
					return c.CompareFiles(outPath, refPath);
			}

			return Default.CompareFiles(outPath, refPath);
		}
	}
}
