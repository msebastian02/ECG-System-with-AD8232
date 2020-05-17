import { SampleData } from './SampleData';

export class Patient {
    name: string;
    address?: string;
    age?: number;
    email?: string;
    phone?: number;
    samples?: SampleData[];
}
