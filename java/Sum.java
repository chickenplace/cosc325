import java.util.Scanner;

public class Sum {

    static int sum(int n) {
        int i;
        int total = 0;
        for (i=1; i<=n; i++) {
            total += i;
        }
        return total;
    }

    public static void main(String args[]) {
        System.out.println("What would you like to sum up to? The program will sum all integers from 1 to your number.");
        Scanner in = new Scanner(System.in);
        int n = Integer.parseInt(in.nextLine());
        System.out.printf("%d\n", sum(n));    
        in.close();
    }

}
