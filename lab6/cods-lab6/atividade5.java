import java.util.Random;

//classe da estrutura de dados (recurso) compartilhado entre as threads
class Acumulador {
	//recurso compartilhado
	private int r;
	//construtor
	public Acumulador() { 
		 this.r = 0; 
	}

	public synchronized void inc() { 
		 this.r++; 
								}

	public synchronized int get() { 
			return this.r; 
	}
	
}

class Contador extends Thread {
	private int id;
	private int nthreads;
	private int[] v;
	private Acumulador acum;

	public Contador(int thread_id, int nthreads, int[] vetor, Acumulador acum) {
		this.id = thread_id;
		this.nthreads = nthreads;
		this.v = vetor;
		this.acum = acum;
	}


	public void run() {
		int idx = this.id;
		int len = this.v.length;

		while (idx < len) {
			if (this.v[idx] % 2 == 0) {
				acum.inc();
			}
			idx += this.nthreads;
		}
	}
}

class Atividade5 {
	static final int tam = 10000;

	public static void main(String[] args) {
		int[] vet = new int[tam];
		Random random = new Random();
		Acumulador acum = new Acumulador();
		int n = 1 + (random.nextInt() % 20); // garante pelo menos uma thread
		n = n >= 0 ? n : -n; // caso n seja negativo
		Thread[] threads = new Thread[n];
		int i;
		int resCorreto = 0;
		int thread;

		System.out.println("Comecou com " + n + " threads");
		
		for (i = 0; i < tam; i++) {
			vet[i] = random.nextInt();
		}

		for (i = 0; i < tam; i++) {
			if (vet[i] % 2 == 0) {
				resCorreto++;
			}
		}

		for (thread = 0; thread < n; thread++) {
			Thread atual = threads[thread] = new Contador(thread, n, vet, acum);
			System.out.println("Comecou thread " + thread);
			atual.start();
		}

		for (thread = 0; thread < n; thread++) {
			try {
				threads[thread].join();
				System.out.println("Coletou thread " + thread);
			} catch (Exception e) {
				System.out.println("Erro Join");
				return;
			}
		}

		System.out.println("Valor correto: " + resCorreto);
		System.out.println("Valor concorrente: " + acum.get());
	}
}
