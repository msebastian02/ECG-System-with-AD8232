import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class ServiceService {
  private URL = 'http://127.0.0.1:5000/';
  constructor(private http: HttpClient) { }
  public getIsLogin(): Observable<any> {
    return this.http.get(this.URL + 'islogin');
  }
  public getLogin(user): Observable<any> {
    return this.http.get(this.URL + 'login?user=' + user);
  }
  public getSignOut(username): Observable<any> {
    return this.http.get(this.URL + 'logout?username=' + username);
  }
  public getUsers(): Observable<any> {
    return this.http.get(this.URL + 'users');
  }
  public getUserInfo(username): Observable<any> {
    return this.http.get(this.URL + 'userinfo?username=' + username);
  }
  public getSamples(): Observable<any> {
    return this.http.get(this.URL + 'samples');
  }
  public postNewSamples(samples): Observable<any> {
    return this.http.get(this.URL + 'newsamples?samples=' + samples);
  }
  public postDeleteSample(sampleId): Observable<any> {
    return this.http.get(this.URL + 'deletesample?sampleid=' + sampleId);
  }
  public postNewUser(userInfo): Observable<any> {
    return this.http.get(this.URL + 'newuser?userinfo=' + userInfo);
  }
  public getData(info): Observable<any> {
    return this.http.get(this.URL + 'data2graph?info=' + info);
  }
  public postDataFile(data): Observable<any> {
    return this.http.get(this.URL + 'datafile?data=' + data);
  }
  public getDataFile(id): Observable<any> {
    return this.http.get(this.URL + 'datafileid?id=' + id);
  }
}
