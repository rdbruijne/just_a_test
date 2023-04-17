using System.Diagnostics;
using System.Reflection.Metadata.Ecma335;
using Patience.Tests;

namespace Patience.Loggers
{
	public class Terminal
	{
		public enum LogLevels
		{
			None,
			Error,
			Warning,
			Info,
			Verbose
		}

		public static LogLevels LogLevel = LogLevels.Info;



		// log a message
		public static void Info(string text) => Message(new string[] { text }, LogLevels.Info);
		public static void Info(IEnumerable<string> text) => Message(text, LogLevels.Info);
		public static void Warning(string text) => Message(new string[] { text }, LogLevels.Warning);
		public static void Warning(IEnumerable<string> text) => Message(text, LogLevels.Warning);
		public static void Error(string text) => Message(new string[] { text }, LogLevels.Error);
		public static void Error(IEnumerable<string> text) => Message(text, LogLevels.Error);
		public static void Verbose(string text) => Message(new string[] { text }, LogLevels.Verbose);
		public static void Verbose(IEnumerable<string> text) => Message(text, LogLevels.Verbose);
		public static void Message(string text, LogLevels level) => Message(new string[] { text, }, level);

		public static void Message(IEnumerable<string> text, LogLevels level)
		{
			if (level == LogLevels.Warning)
				Console.ForegroundColor = ConsoleColor.Yellow;
			else if (level == LogLevels.Error)
				Console.ForegroundColor = ConsoleColor.Red;

			foreach (string line in text)
				Console.WriteLine(line);

			Console.ResetColor();
		}



		// write an empty line
		public static void EmptyLine() => Console.WriteLine();



		// ask a questions
		public static char Ask(string question)
		{
			Console.Write(question);
			ConsoleKeyInfo keyInfo = Console.ReadKey();
			char result = keyInfo.KeyChar;
			Console.WriteLine();
			return result;
		}



		// halt until the user provides input
		public static void WaitForInput()
		{
			Console.ReadLine();
		}



		// move the cursor back to the beginning of the previous line
		//public static void UndoNewline()
		//{
		//}



		// show if the result was a success or failure
		public static void AppendRunResult(string message, bool success)
		{
			Console.SetCursorPosition(message.Length, Console.CursorTop - 1);
			if (success)
			{
				Console.ForegroundColor = ConsoleColor.Green;
				Console.WriteLine($" \u221A "); // sqrt character
			}
			else
			{
				Console.ForegroundColor = ConsoleColor.Red;
				Console.WriteLine(" X ");
			}
			Console.ResetColor();
		}
	}
}
