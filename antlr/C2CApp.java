// import ANTLR runtime libraries
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.NotNull;
import org.antlr.v4.runtime.tree.*;

import java.io.*;
import java.util.*;

class C2CListener extends CBaseListener 
{
	Stack<StringBuilder> out = new Stack<StringBuilder>();
	CParser parser;
	C2CListener(CParser parser){
		out.push(new StringBuilder(""));
		this.parser = parser;
	}
	
	@Override	public void enterDirectiveDefinition(@NotNull CParser.DirectiveDefinitionContext ctx) {
		if(ctx.getParent() instanceof CParser.IncListContext)
			for (int i=0; i< ctx.getChildCount(); i++)
				out.peek().append(ctx.getChild(i).getText()+"\n");	// Print IncList
	}
	
	@Override	public void enterFunction(@NotNull CParser.FunctionContext ctx) { 
		for (int i=0; i < ctx.getChildCount(); i++)
			if (ctx.getChild(i) instanceof TerminalNode)
				out.peek().append(ctx.getChild(i).getText()+" ");		// Print Function without CompoundStmt
	}
	
	@Override	public void visitTerminal(TerminalNode node) {
		if(node.getParent().getParent().getParent() instanceof CParser.ProgramContext==false)
			out.peek().append(node.getText()+'\n'); 			// Print TerminalNode 
	}
}

public class C2CApp {
	public static void main(String[] args) throws IOException {
		// Input file		
		if (args.length != 1){
			System.err.println("Input C file");
			System.exit(1);
		}
		String filename = args[0];
		System.setIn(new FileInputStream(filename)); 
		InputStream input = System.in;

		// Get lexer
		CLexer lexer = new CLexer(new ANTLRInputStream(input));
		// Get a list of matched tokens
		CommonTokenStream tokens = new CommonTokenStream(lexer);
		// Pass tokens to parser
		CParser parser = new CParser(tokens);
		// Walk parse-tree and attach our listener
		ParseTreeWalker walker = new ParseTreeWalker();
		C2CListener listener = new C2CListener(parser);
		// walk from the root of parse tree
		walker.walk(listener, parser.program());
	
		// Output file
		FileOutputStream output = new FileOutputStream(new File("output_"+args[0]));
		System.out.println("Output file name:  output_"+args[0]);
		output.write(listener.out.peek().toString().getBytes());
		output.flush();
		output.close();
	}
} 
