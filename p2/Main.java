
class Main {

    public static void main(String[] args) {
        Thread hilo1 = new Hilo();
        Thread hilo2 = new Thread(new Ejecutable());
        hilo1.setPriority(1);

        hilo1.start();
        hilo2.start();
    }

    static class Funcionalidad {
        private String palabra;

        private int veces;
        
        public Funcionalidad(String p) {
            palabra = p;
            veces = 10;
        }

        public Funcionalidad(String p, int v) {
            palabra = p;
            veces = v;
        }

        public void actuar() {
            for(int i=0; i<veces; ++i) {
                System.out.println(palabra);
            }
        }
    }

    static class Hilo extends Thread {
        public void run() {
            new Funcionalidad("soy hilo por herencia").actuar();
        }
    }

    static class Ejecutable implements Runnable {
        public void run() {
            new Funcionalidad("soy hilo por interfaz").actuar();
        }
    }

}