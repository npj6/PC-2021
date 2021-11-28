
class Peterson {

    static boolean[] cs_ready = {false, false};
    static int turn = 0;
    static int I = 0;
    
    static int VALUE = 20000000;

    public static void main(String[] args) {
        Thread hilo1 = new Thread(new Ejecutable(0));
        Thread hilo2 = new Thread(new Ejecutable(1));
        
        hilo1.start();
        hilo2.start();
        try {
        	hilo1.join();
        } catch (InterruptedException e) {}
        
        try {
        	hilo2.join();
        } catch (InterruptedException e) {}
        
        System.out.println("I vale "+ Integer.toString(I) +", deberia valer "+ Integer.toString(VALUE*2) +"\n");
    }

    static class Ejecutable implements Runnable {
    	private int yo;
    	private int otro;
    
    	public Ejecutable(int yo) {
    		this.yo = yo;
    		this.otro = 1 - yo;
    	}
    
    	public void pre_cs() {
    		System.out.println("pre_cs de " + Integer.toString(yo));
    		cs_ready[yo] = true;
    		turn = otro;
    		while (cs_ready[otro] && turn == otro);
    	}
    	
    	public void post_cs() {
    		System.out.println("post_cs de " + Integer.toString(yo));
    		cs_ready[yo] = false;
    	}
    
        public void run() {
        	pre_cs();
            
    		System.out.println("cs de " + Integer.toString(yo));
       	for(int i=0; i<VALUE; ++i)
            	++I;
            
            	post_cs();
        }
    }

}
