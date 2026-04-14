package com.ding.rtc.demo.ui;

import android.content.Context;
import android.graphics.PointF;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.VelocityTracker;

public class GestureHandler implements ScaleGestureDetector.OnScaleGestureListener {
    private static final float MAX_CLICK_DISTANCE = 20.0f;
    private static final float MAX_DBLCLICK_DISTANCE = 50.0f;
    private static final int MAX_CLICK_INTERVAL = 300;
    private static final int MAX_DBLCLICK_INTERVAL = 500;

    private PointF downPoint_ = null;
    private PointF lastPoint_ = null; // for calc relative distance when move
    private float maxDistance_ = 0.0f;
    private int pointerId_ = -1;

    private PointF downPoint2_ = null;
    private int pointer2Id_ = -1;
    private float downPointDistance_ = 0.0f;

    private PointF clickPoint_ = null; // for detecting double click
    private long clickTime_ = 0;
    private ActionType action_ = ActionType.Null;
    private int pointerCount_ = 0;
    private int maxPointers_ = 0;

    private VelocityTracker velocityTracker_;

    private final ScaleGestureDetector scaleDetector_;
    private boolean scaleGesture_ = false;

    private final Callback cb_;

    public enum ActionType { Drag, Scroll, Move, Click, RightClick, DoubleClick, End, Null }

    public GestureHandler(Context ctx, Callback cb) {
        cb_ = cb;

        scaleDetector_ = new ScaleGestureDetector(ctx, this);
    }

    public Boolean handleEvent(MotionEvent event) {
        scaleDetector_.onTouchEvent(event);
        if(scaleDetector_.isInProgress()) {
            return true;
        }
        return handleTouch(event);
    }

    @Override
    public boolean onScaleBegin(ScaleGestureDetector detector) {
        return scaleGesture_;
    }
    @Override
    public boolean onScale(ScaleGestureDetector detector) {
        cb_.onScale(detector.getScaleFactor(), detector.getFocusX(), detector.getFocusY(), detector.getCurrentSpanX(), detector.getCurrentSpanY());
        return true;
    }
    @Override
    public void onScaleEnd(ScaleGestureDetector detector) {
    }

    private boolean handleTouch(MotionEvent event) {
        int maskedAction = event.getActionMasked();
        switch (maskedAction) {
            case MotionEvent.ACTION_DOWN: {
                int pointerIndex = event.getActionIndex();
                pointerId_ = event.getPointerId(pointerIndex);
                downPoint_ = new PointF();
                downPoint_.x = event.getX(pointerIndex);
                downPoint_.y = event.getY(pointerIndex);
                pointer2Id_ = -1;
                lastPoint_ = new PointF(downPoint_.x, downPoint_.y);
                maxDistance_ = 0.0f;
                pointerCount_ = 1;
                maxPointers_ = 1;
                scaleGesture_ = false;
                action_ = ActionType.Null;

                if(velocityTracker_ == null) {
                    velocityTracker_ = VelocityTracker.obtain();
                } else {
                    velocityTracker_.clear();
                }
                velocityTracker_.addMovement(event);
                cb_.onBegin(downPoint_.x, downPoint_.y);
                break;
            }
            case MotionEvent.ACTION_POINTER_DOWN: {
                // We have a new pointer. Lets add it to the list of pointers
                ++pointerCount_;
                if (pointerCount_ == 2) {
                    int pointerIndex = event.getActionIndex();
                    pointer2Id_ = event.getPointerId(pointerIndex);
                    downPoint2_ = new PointF();
                    downPoint2_.x = event.getX(pointerIndex);
                    downPoint2_.y = event.getY(pointerIndex);
                    float dx = downPoint2_.x - downPoint_.x;
                    float dy = downPoint2_.y - downPoint_.y;
                    downPointDistance_ = (float) Math.sqrt(dx * dx + dy * dy);
                }
                ++maxPointers_;
                cb_.onCancel();
                break;
            }
            case MotionEvent.ACTION_MOVE: { // a pointer was moved
                if(lastPoint_ == null || downPoint_ == null) {
                    break;
                }
                int pointerIndex = event.findPointerIndex(pointerId_);
                if (-1 == pointerIndex) {
                    break;
                }
                velocityTracker_.addMovement(event);
                float x = event.getX(pointerIndex);
                float y = event.getY(pointerIndex);
                float dx = x - downPoint_.x;
                float dy = y - downPoint_.y;
                ActionType action = action_;
                if (action_ == ActionType.Null) {
                    // detect action type
                    int pointerIndex2 = event.findPointerIndex(pointer2Id_);
                    if (-1 != pointerIndex2) {
                        float x2 = event.getX(pointerIndex2);
                        float y2 = event.getY(pointerIndex2);
                        float dx2 = x2 - x;
                        float dy2 = y2 - y;
                        float distance = (float) Math.sqrt(dx2 * dx2 + dy2 * dy2);
                        if (Math.abs(distance - downPointDistance_) >= MAX_CLICK_DISTANCE) {
                            scaleGesture_ = true;
                            break;
                        }
                    }
                    float d = (float) Math.sqrt(dx * dx + dy * dy);
                    if (d > maxDistance_) {
                        maxDistance_ = d;
                    }
                    if (maxDistance_ >= MAX_CLICK_DISTANCE) {
                        if (pointerCount_ == 2) {
                            action_ = ActionType.Scroll;
                        } else if (pointerCount_ == 3) {
                            action_ = ActionType.Drag;
                        } else {
                            action_ = ActionType.Move;
                        }
                        action = action_;
                    } else {
                        action = ActionType.Move;
                    }
                }
                velocityTracker_.computeCurrentVelocity(1000);
                float vx = velocityTracker_.getXVelocity(pointerId_);
                float vy = velocityTracker_.getYVelocity(pointerId_);
                dx = x - lastPoint_.x;
                dy = y - lastPoint_.y;
                lastPoint_.x = x;
                lastPoint_.y = y;
                if (action == ActionType.Drag) {
                    cb_.onDrag(x, y, dx, dy, vx, vy);
                } else if (action == ActionType.Scroll) {
                    cb_.onScroll(x, y, dx, dy, vx, vy);
                } else {
                    cb_.onMove(x, y, dx, dy, vx, vy);
                }
                break;
            }
            case MotionEvent.ACTION_UP: {
                if (downPoint_ == null) {
                    break;
                }
                int pointerIndex = event.getActionIndex();
                float x = event.getX(pointerIndex);
                float y = event.getY(pointerIndex);
                //if(pointerId == pointerId_) {
                ActionType action = ActionType.End;
                long downTime = event.getDownTime();
                long upTime = event.getEventTime();
                if (maxDistance_ < MAX_CLICK_DISTANCE && upTime - downTime < MAX_CLICK_INTERVAL) {
                    action = ActionType.Click; // click
                    if(maxPointers_ == 2) {
                        action = ActionType.RightClick;
                    } else if (clickPoint_ != null && downTime - clickTime_ < MAX_DBLCLICK_INTERVAL) {
                        float dx = downPoint_.x - clickPoint_.x;
                        float dy = downPoint_.y - clickPoint_.y;
                        float d = (float) Math.sqrt(dx * dx + dy * dy);
                        if (d < MAX_DBLCLICK_DISTANCE) {
                            action = ActionType.DoubleClick;
                        }
                    }
                    if (action == ActionType.Click) {
                        clickPoint_ = new PointF(x, y);
                        clickTime_ = upTime;
                    } else {
                        clickPoint_ = null;
                        clickTime_ = 0;
                    }
                } else {
                    clickPoint_ = null;
                    clickTime_ = 0;
                }
                if (action == ActionType.RightClick) {
                    cb_.onRightClicked(x, y);
                } else if (action == ActionType.DoubleClick) {
                    cb_.onDoubleClicked(x, y);
                } else if (action == ActionType.Click) {
                    cb_.onClicked(x, y);
                } else {
                    cb_.onEnd(x, y);
                }
                clearMouseAction();
                //}
                break;
            }
            case MotionEvent.ACTION_POINTER_UP: {
                --pointerCount_;
                int pointerIndex = event.getActionIndex();
                int pointerId = event.getPointerId(pointerIndex);
                if (pointerCount_ == 1) {
                    downPoint2_ = null;
                    pointer2Id_ = -1;
                }
                int pointerCount = event.getPointerCount();
                if(pointerId == pointerId_) {
                    int newPointerIndex = pointerIndex == 0 ? 1 : 0;
                    newPointerIndex = Math.min(newPointerIndex, pointerCount-1);
                    lastPoint_ = new PointF(event.getX(newPointerIndex),
                            event.getY(newPointerIndex));
                    pointerId_ = event.getPointerId(newPointerIndex);
                    if (pointerCount_ >= 2) {
                        newPointerIndex = pointerIndex == 0 ? 2 : 1;
                        newPointerIndex = Math.min(newPointerIndex, pointerCount-1);
                        downPoint2_ = new PointF(event.getX(newPointerIndex),
                                event.getY(newPointerIndex));
                        pointer2Id_ = event.getPointerId(newPointerIndex);
                    }
                }
                if (pointerId == pointer2Id_ && pointerCount_ >= 2) {
                    int newPointerIndex = pointerIndex == 1 ? 2 : 1;
                    newPointerIndex = Math.min(newPointerIndex, pointerCount-1);
                    downPoint2_ = new PointF(event.getX(newPointerIndex),
                            event.getY(newPointerIndex));
                    pointer2Id_ = event.getPointerId(newPointerIndex);
                }
                break;
            }
            case MotionEvent.ACTION_CANCEL: {
                if (lastPoint_ != null) {
                    cb_.onEnd(lastPoint_.x, lastPoint_.y);
                }
                break;
            }
            default:
                break;
        }
        return true;
    }

    private void clearMouseAction() {
        downPoint_ = null;
        lastPoint_ = null;
        pointerId_ = -1;
        downPoint2_ = null;
        pointer2Id_ = -1;
        downPointDistance_ = 0.0f;
        maxDistance_ = 0.0f;
        action_ = ActionType.Null;
        pointerCount_ = 0;
        maxPointers_ = 0;
        scaleGesture_ = false;
        if (velocityTracker_ != null) {
            velocityTracker_.recycle();
            velocityTracker_ = null;
        }
    }

    public interface Callback {
        default void onBegin(float x, float y) {}
        default void onClicked(float x, float y) {}
        default void onRightClicked(float x, float y) {}
        default void onDoubleClicked(float x, float y) {}
        default void onMove(float x, float y, float dx, float dy, float vx, float vy) {}
        default void onDrag(float x, float y, float dx, float dy, float vx, float vy) {}
        default void onScroll(float x, float y, float dx, float dy, float vx, float vy) {}
        default void onScale(float factor, float focusX, float focusY, float spanX, float spanY) {}
        default void onEnd(float x, float y) {}
        default void onCancel() {}
    }
}
