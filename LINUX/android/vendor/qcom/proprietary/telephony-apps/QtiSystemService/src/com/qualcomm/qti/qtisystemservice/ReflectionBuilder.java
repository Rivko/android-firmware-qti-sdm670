/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
package com.qualcomm.qti.qtisystemservice;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class ReflectionBuilder {

    static class Reflection<T> {
        T t;
        public Reflection(T t) {
            this.t = t;
        }

        public Object invoke(Object receiver, Object ...arg) {
            if (t instanceof Method) {
                Method method = (Method) t;
                if (!method.isAccessible()) {
                    method.setAccessible(true);
                }
                try {
                    return method.invoke(receiver, arg);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
            } else if (t instanceof Field) {
                Field field = (Field) t;
                try {
                    return field.get(receiver);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                }
            }
            return null;
        }
    }

    static class ReflectionMethod <T> extends Reflection<T> {
        private Class<?> mCls;

        public ReflectionMethod(T t, Class<?> cls) {
            super(t);
            mCls = cls;
        }

        public Object callWithoutReceiver(Object ...arg) {
            try {
                return invoke(mCls.newInstance(), arg);
            } catch (InstantiationException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            }
            return null;
        }

        public Object call(Object receiver, Object ...arg) {
            return invoke(receiver, arg);
        }

        public Object call(Object receiver, String methodName, Object ...arg) {
            try {
                Method method = mCls.getDeclaredMethod(methodName);
                if (!method.isAccessible()) {
                    method.setAccessible(true);
                }
                return method.invoke(receiver,arg);
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            }
            return null;
        }
    }

    static class ReflectionField <T> extends Reflection<T> {
        private Class<?> mCls;

        public ReflectionField(T t, Class<?> cls) {
            super(t);
        }

        public Object get(Object receiver) {
            return invoke(receiver);
        }

        public void setAccess(Object receiver,boolean access) {
            Field field = (Field) t;
            if (field.isAccessible() != access) {
                field.setAccessible(access);
            }
        }

        public void setValue(Object receiver, int value) {
            Field field = (Field) t;
            if (field.isAccessible()) {
                try {
                    field.setInt(receiver, value);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public static ReflectionMethod buildMethod(ClassLoader cl, String className,
            String methodName, Class<?> ...args) {
        try {
            Class<?> classForMethod = Class.forName(className, false, cl);
            Method method = classForMethod.getDeclaredMethod(methodName, args);
            return  new ReflectionBuilder.ReflectionMethod<Method>(method, classForMethod);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
        return  null;
    }

    public static ReflectionMethod buildMethod(ClassLoader cl, String className) {
        try {
            Class<?> classForMethod = Class.forName(className, false, cl);
            return  new ReflectionBuilder.ReflectionMethod<Method>(null, classForMethod);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
        return  null;
    }

    public static ReflectionField buildField(ClassLoader cl, String className, String fieldName) {
        try {
            Class<?> classForMethod = Class.forName(className, false, cl);
            Field field = classForMethod.getDeclaredField(fieldName);
            return  new ReflectionBuilder.ReflectionField<Field>(field, classForMethod);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
        return  null;
    }

    public static ReflectionField buildField(ClassLoader cl, String className) {
        try {
            Class<?> classForMethod = Class.forName(className, false, cl);
            return  new ReflectionBuilder.ReflectionField<Field>(null, classForMethod);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
        return  null;
    }
}
