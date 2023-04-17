namespace Patience.Comparing
{
	internal interface ICompare
	{
		bool SupportsType(string ext);
		List<string> SupportedTypes();
		bool CompareFiles(string firstPath, string secondPath);
	}
}
