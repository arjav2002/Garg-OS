import java.util.*;
import java.io.*;
public class ReadFile {
	public static void main(String[] args) throws Exception {
		Scanner sc = new Scanner(new File("outfile.txt"));
		LinkedList<String> list = new LinkedList<String>();
		while(sc.hasNextLine()) list.add(sc.nextLine());
		for(int i = 0; i < list.size(); i++) {
			int pos = i;
			int val = 0;
			try {val = Integer.parseInt(list.get(i).split(" ")[0], 16); } catch(NumberFormatException e) { continue; }
			for(int j = i+1; j < list.size(); j++) {
				int thisVal = 0;
				try { thisVal = Integer.parseInt(list.get(j).split(" ")[0], 16); } catch(NumberFormatException e) { continue; }
				if(thisVal < val) {
					pos = j;
					val = thisVal;
				}
			}
			String str = list.get(i);
			list.set(i, list.get(pos));
			list.set(pos, str);
		}

		for(String str : list) System.out.println(str);
		sc.close();
	}
}
