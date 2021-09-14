import java.lang.Math;
import java.util.concurrent.atomic.AtomicInteger;

// Monitor que implementa a logica do padrao leitores/escritores
class Monitor {
	private int leit, escr;  
	
	// Construtor
	Monitor() { 
		 this.leit = 0; //leitores lendo (0 ou mais)
		 this.escr = 0; //escritor escrevendo (0 ou 1)
	} 
	
	// Entrada para leitores
	public synchronized void EntraLeitor (int id) {
		try { 
			while (this.escr > 0) {
				 System.out.println ("le.leitorBloqueado("+id+")");
				 wait();	//bloqueia pela condicao logica da aplicacao 
			}
			this.leit++;	//registra que ha mais um leitor lendo
			System.out.println ("le.leitorLendo("+id+")");
		} catch (InterruptedException e) { }
	}
	
	// Saida para leitores
	public synchronized void SaiLeitor (int id) {
		 this.leit--; //registra que um leitor saiu
		 if (this.leit == 0) 
					 this.notify(); //libera escritor (caso exista escritor bloqueado)
		 System.out.println ("le.leitorSaindo("+id+")");
	}
	
	// Entrada para escritores
	public synchronized void EntraEscritor (int id) {
		try { 
			while ((this.leit > 0) || (this.escr > 0)) {
			//if ((this.leit > 0) || (this.escr > 0)) {
				 System.out.println ("le.escritorBloqueado("+id+")");
				 wait();	//bloqueia pela condicao logica da aplicacao 
			}
			this.escr++; //registra que ha um escritor escrevendo
			System.out.println ("le.escritorEscrevendo("+id+")");
		} catch (InterruptedException e) { }
	}
	
	// Saida para escritores
	public synchronized void SaiEscritor (int id) {
		 this.escr--; //registra que o escritor saiu
		 notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
		 System.out.println ("le.escritorSaindo("+id+")");
	}
}

// Leitor
class Leitor extends Thread {
	int id; //identificador da thread
	Monitor monitor;//objeto monitor para coordenar a lógica de execução das threads
	AtomicInteger num;

	// Construtor
	Leitor (int id, Monitor m, AtomicInteger n) {
		this.id = id;
		this.monitor = m;
		this.num = n;
	}

	// Método executado pela thread
	public void run () {
		this.monitor.EntraLeitor(this.id);
		int val = this.num.intValue();
		boolean primo = false;
		if (val > 1) {
				primo = true;
				for (int d = 2; d < Math.round(Math.sqrt(val)); d++) {
						if (val % d == 0) {
								primo = false;
						}
				}
		}
		String pref = String.format("Thread_ID: (%d)\t", this.id);
		if (primo) {
				System.out.println(pref + val + " é primo!");
		} else {
				System.out.println(pref + val + " não é primo...");
		}
		this.monitor.SaiLeitor(this.id);
	}
}

// Leitor-Escritor
class LeitorEscritor extends Thread {
	int id; //identificador da thread
	Monitor monitor;//objeto monitor para coordenar a lógica de execução das threads
	AtomicInteger num;

	// Construtor
	LeitorEscritor (int id, Monitor m, AtomicInteger n) {
		this.id = id;
		this.monitor = m;
		this.num = n;
	}

	// Método executado pela thread
	public void run () {
		int val;
		this.monitor.EntraLeitor(this.id);
		val = this.num.intValue();
		String pref = String.format("Thread_ID: (%d)\t", this.id);
		if (val % 2 == 0) {
				System.out.println(pref + val + " é par.");
		} else {
				System.out.println(pref + val + " é ímpar.");
		}
		this.monitor.SaiLeitor(this.id);
		this.monitor.EntraEscritor(this.id);
		val = this.num.intValue();
		System.out.println(pref + val+" -> "+(val*2));
		this.num.set(val*2);
		this.monitor.SaiEscritor(this.id);
	}
}

//--------------------------------------------------------
// Escritor
class Escritor extends Thread {
	int id; //identificador da thread
	Monitor monitor; //objeto monitor para coordenar a lógica de execução das threads
	AtomicInteger num;

	// Construtor
	Escritor (int id, Monitor m, AtomicInteger n) {
		this.id = id;
		this.monitor = m;
		this.num = n;
	}

  // Método executado pela thread
  public void run () {
		this.monitor.EntraEscritor(this.id); 
		String pref = String.format("Thread_ID: (%d)\t", this.id);
		int val = this.num.intValue();
		System.out.println(pref + val+" -> "+id);
		this.num.set(id);
		this.monitor.SaiEscritor(this.id); 
  }
}

public class Principal {
	static final int E = 2;
	static final int L = 10;
	static final int LE = 3;

	public static void main(String[] args) {
		AtomicInteger db = new AtomicInteger(0);
		Leitor[] leitores = new Leitor[L];
		Escritor[] escritores = new Escritor[E];
		LeitorEscritor[] leitoresEscritores = new LeitorEscritor[LE];
		Monitor monitor = new Monitor();
		int e, l, le, cnt;
		e = l = le = cnt = 0;

		/*
		while ((E - e) > 0 || (L - l) > 0 || (LE - le) > 0) {
				if (le < LE) {
						leitoresEscritores[le] = new LeitorEscritor(cnt++, monitor, db);
						leitoresEscritores[le].start();
						le += 1;
				}
				if (e < E) {
						escritores[e] = new Escritor(cnt++, monitor, db);
						escritores[e].start();
						e += 1;
				}
				if (l < L) {
						leitores[l] = new Leitor(cnt++, monitor, db);
						leitores[l].start();
						l += 1;
				}
		}
		*/
		for (Escritor escr : escritores) {
				escr = new Escritor(cnt++, monitor, db);
				escr.start();
				e += 1;
		}
		for (Leitor leit : leitores) {
				leit = new Leitor(cnt++, monitor, db);
				leit.start();
				l += 1;
		}
		for (LeitorEscritor leitEscr : leitoresEscritores) {
				leitEscr = new LeitorEscritor(cnt++, monitor, db);
				leitEscr.start();
				le += 1;
		}
	}
}
