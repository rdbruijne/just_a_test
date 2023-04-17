namespace Patience.Comparing
{
	internal class DefaultCompare : ICompare
	{
		public bool SupportsType(string ext)
		{
			return true;
		}



		public List<string> SupportedTypes()
		{
			return new List<string>();
		}



		public bool CompareFiles(string firstPath, string secondPath)
		{
			// compare file sizes
			FileInfo[] info = { new(firstPath), new(secondPath) };
			if (info[0].Length != info[1].Length)
				return false;

			// chunk-wise binary compare
			const int CHUNK_SIZE = sizeof(Int64);
			using FileStream stream0 = info[0].OpenRead();
			using FileStream stream1 = info[1].OpenRead();
			byte[][] chunks = { new byte[CHUNK_SIZE], new byte[CHUNK_SIZE] };
			long iterations = (info[0].Length + CHUNK_SIZE - 1) / CHUNK_SIZE;
			for (long i = 0; i < iterations; i++)
			{
				stream0.Read(chunks[0], 0, CHUNK_SIZE);
				stream1.Read(chunks[1], 0, CHUNK_SIZE);
				if (BitConverter.ToUInt64(chunks[0], 0) != BitConverter.ToUInt64(chunks[1], 0))
					return false;
			}

			return true;
		}
	}
}
