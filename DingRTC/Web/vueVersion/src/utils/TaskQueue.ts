interface INamedTask {
  taskName: string;
  task: any;
}

export class TaskQueue {
  queueName: string;
  taskArray: INamedTask[];
  acc: number;
  constructor(queueName: string) {
    this.queueName = queueName;
    this.taskArray = [];
    this.acc = 0;
  }

  public queueTask(task: () => any) {
    this.acc = this.acc + 1;
    const namedTask = { task, taskName: `t${this.acc}` };
    this.taskArray.push(namedTask);
    if (this.taskArray.length === 1) {
      this.triggerExec();
    }
  }

  private async triggerExec() {
    let namedTask: any;
    /**
     * NOTE 注意不要先shift，这样可能导致有一个正在执行的task，但是queue size为0，导致新来的task也会触发
     */
    while ((namedTask = this.taskArray[0])) {
      console.log('qname=', this.queueName, 'start taskName', namedTask?.taskName);
      try {
        const t = namedTask.task;
        await t();
      } catch (e) {
        console.log(`qname=${this.queueName} task error`, e && e.name, e && e.message);
      } finally {
        console.log('qname=', this.queueName, 'finished taskName', namedTask?.taskName);
        this.taskArray.shift();
      }
    }
  }

  public clearTasks() {
    this.taskArray = [];
  }
}
