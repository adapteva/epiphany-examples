//package epiphanymonitor2;

/**
 *
 * @author Kevin Cheng
 */
import java.awt.*;
import java.awt.geom.*;
import javax.swing.*;
import java.io.FileNotFoundException;
import java.io.File;
import java.util.*;
import java.util.Scanner;
import java.io.*;

public class EpiphanyMonitor2 extends JFrame{

    public static int[] ecoreLevel = new int[16];
    public static int[] emeshLevel = new int[16];
    public static int row;
    public static int col;
    public static Rectangle2D[] ecoreRect = new Rectangle2D[16];
    public static Rectangle2D[] emeshRect = new Rectangle2D[16];
    public static Color[] cs = new Color[32];

    public EpiphanyMonitor2(int colors)
    {
        for (int i = 0; i < colors; i++)
        {
            int red   = i * (256 / colors);
            int green = 255 - red;
            int blue  = 0;
            cs[i] = new Color(red, green, blue);
        }
    }

    public static void main(String[] args)
    {
        String ecorePathname = "/home/linaro/ecore.status";
        String emeshPathname = "/home/linaro/emesh.status";
        int numOfColors = 32;
        new EpiphanyMonitor2(numOfColors);
        int width = 470;
        int height = 242;
        int refreshRate = 200;

        readEcore(ecorePathname);
        readEmesh(emeshPathname);

        JFrame ecoreFrame = new JFrame("eCore Load                           eMesh Load");
        ecoreFrame.setSize(width, height);
        scale map = new scale();
        ecoreFrame.add(map);
        ecoreFrame.setVisible(true);

        ecoreFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        map = null;

        while (true) {
            readEcore(ecorePathname);
            readEmesh(emeshPathname);
            map = new scale();
            ecoreFrame.add(map);
            ecoreFrame.repaint();
            map = null;

            try {
                Thread.sleep(refreshRate);
            } catch (InterruptedException e) {

            }
        }
    }

    public static class scale extends JPanel {

        public void paintComponent(Graphics comp) {
            Graphics2D comp2D = (Graphics2D) comp;
            double up = 10;
            int color = 31;
            int numOfColors = 32;
            int scaleX = 25;
            int scaleY = 6;
            int row = 4;
            int col =4 ;
            for (int x = 0; x < numOfColors; x++) {
                comp2D.setColor(cs[color]);
                Rectangle2D.Double e1 = new Rectangle2D.Double(217.5, up, scaleX, scaleY);
                comp2D.fill(e1);
                color--;
                up = up + 5.93;
            }
            Graphics2D ecore = (Graphics2D) comp;
            int core;
            int counter = 0;
            int corecolor;

            for (int x = 0; x < row; x++) {
                for (int y = 0; y < col; y++) {
                    ecore.setColor(cs[ecoreLevel[counter]]);
                    Rectangle2D.Double rect = new Rectangle2D.Double(10 + 50 * x, 10 + 50 * y, 40, 40);
                    ecore.fill(rect);
                    counter++;
                }
            }
            Graphics2D emesh = (Graphics2D) comp;
            int mesh;
            counter = 0;
            int meshcolor;
            for (int x = 0; x < row; x++) {
                for (int y = 0; y < col; y++) {
                    emesh.setColor(cs[emeshLevel[counter]]);
                    Rectangle2D.Double rect = new Rectangle2D.Double(250 + 10 + 50 * x, 10 + 50 * y, 40, 40);
                    emesh.fill(rect);
                    emeshRect[counter] = rect;
                    counter++;
                }
            }
        }
    }

    public static void readEcore(String path) {
        String ecorePathname = path;
        File file = new File(ecorePathname);
        Scanner input = null;
        String word;
        int index = 0;
        try {
            input = new Scanner(file);
            while (input.hasNext()) {
                row = input.nextInt();
                col = input.nextInt();
                ecoreLevel[index] = input.nextInt();
                index++;
            }

        } catch (FileNotFoundException ex) {
            System.out.println("*** Cannot open " + ecorePathname + " ***");
            System.exit(1);  // quit the program
        }
        input.close();

    }

    public static void readEmesh(String path) {
        String emeshPathname = path;
        File file = new File(emeshPathname);
        Scanner input = null;
        String word;
        int index = 0;
        try {
            input = new Scanner(file);
            while (input.hasNext()) {
                row = input.nextInt();
                col = input.nextInt();
                emeshLevel[index] = input.nextInt();
                index++;
            }

        } catch (FileNotFoundException ex) {
            System.out.println("*** Cannot open " + emeshPathname + " ***");
            System.exit(1);  // quit the program
        }

        input.close();
    }
}
