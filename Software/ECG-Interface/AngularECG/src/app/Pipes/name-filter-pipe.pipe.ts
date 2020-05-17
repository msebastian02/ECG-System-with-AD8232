import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'nameFilterPipe'
})
export class NameFilterPipePipe implements PipeTransform {

  transform(users: any[], filtername: string): any {
    if (!users || !filtername) {
      return users;
    }
    let result = [];
    users.forEach(u => {
      if (this.ofAccent(u.name).toLowerCase().indexOf(filtername.toLocaleLowerCase()) !== -1) {
        result.push(u);
      }
    });
    return result;
    }

    private ofAccent(input) {
        const accent = "ÃÀÁÄÂÈÉËÊÌÍÏÎÒÓÖÔÙÚÜÛãàáäâèéëêìíïîòóöôùúüûÑñÇç";
        const original = "AAAAAEEEEIIIIOOOOUUUUaaaaaeeeeiiiioooouuuunncc";
        for (let i = 0; i < accent.length; i++) {
            input = input.replace(accent.charAt(i), original.charAt(i)).toLowerCase();
        }
        return input;
    }

}

